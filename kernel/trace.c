#include <assert.h>
#include <errno.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/trace.h>

/*
 * Trace is a ptrace-style interface for nightingale, offering debugging
 * and process management capabilties. I haven't used the same name
 * just because I didn't design this promise 1:1 compatability with
 * Linux ptrace but that I am open to revisiting that decision.
 *
 * At the moment Nightingale trace, Processes trace Threads, so any
 * thread in a tracing process may issue commands to a traced thread.
 *
 * Like ptrace, Nightingale trace allows a tracing process to receive, inspect,
 * change, and suppress signal delivery. Calling trace(2) with any of the
 * resuming arguments (TR_SINGLESTEP, TR_SYSCALL, TR_CONT) and a non-zero signal
 * in the `data` argument will cause that signal to be delivered to the traced
 * thread.
 *
 * Trace supports the following top-level commands:
 *
 * TR_TRACEME:
 * Stops the running thread and sets the tracer to the running process's
 * parent.
 *
 * TR_ATTACH:
 * Claims a particular thread for tracing. At this time, there is no
 * security or permissions checks on this, any process may claim any
 * thread for tracing at any time. In future, this will be extended to
 * respect some combination of user ownership, sub-tree relationship,
 * or other superuser permissions
 *
 * TR_GETREGS / TR_SETREGS:
 * Get/set the user_ctx of a thread. This is only valid when the thread
 * is stopped in one of the TRACE_*_STOP states, otherwise there may either be
 * no user_ctx to read or you may race updating it with something else
 *
 * TR_READMEM / TR_WRITEMEM:
 * Return ETODO, not yet implemented. I don't currently have a good
 * way to create views on arbitrary memory ranges outside of the current
 * virtual memory context, so this is waiting on more complex virtual memory
 * object management
 *
 * TR_SINGLESTEP:
 * Resume a stopped thread with the CPU's trap flag set, which will return the
 * traced thread to TRACE_TRAPPED after excecuting a single CPU instruction.
 * This is exercized in the standard distribution by the `step` user program.
 *
 * TR_SYSCALL:
 * Resume a stopped thread until its next syscall, at which point it will move
 * to TRACE_SYSCALL_ENTER_STOP
 *
 * TR_CONT:
 * Resume a stopped thread without asking for singlestep or syscall
 * notifications. Tracer will still receive signal notifications and can still
 * manage signal delivery
 *
 * TR_DETACH:
 * trace(TR_DETACH, tid, 0, 0) removes any trace on `tid`. It can be called by
 * anyone in the system, including the tracing process and the traced thread.
 * This is another place where security and scoping restrictions are needed
 */

static void wake_tracer_with(struct thread *tracee, int value);

bool trace_is_stopped(struct thread *th) {
	return (th->trace_state == TRACE_SYSCALL_ENTER_STOP
		|| th->trace_state == TRACE_SYSCALL_EXIT_STOP
		|| th->trace_state == TRACE_SIGNAL_DELIVERY_STOP
		|| th->trace_state == TRACE_TRAPPED);
}

static sysret trace_traceme() {
	struct process *parent = running_process->parent;
	running_thread->trace_state = TRACE_RUNNING;
	running_thread->tracer = parent;
	list_append(&parent->tracees, &running_addr()->trace_node);
	return 0;
}

static sysret trace_attach(struct thread *th) {
	if (!th)
		return -ESRCH;
	th->tracer = running_process;
	th->trace_state = TRACE_RUNNING;
	list_append(&running_process->tracees, &th->trace_node);
	return 0;
}

static sysret trace_getregs(struct thread *th, void *data) {
	if (!th)
		return -ESRCH;
	if (!trace_is_stopped(th))
		return -EINVAL;
	memcpy(data, th->user_ctx, sizeof(interrupt_frame));
	return 0;
}

static sysret trace_setregs(struct thread *th, void *data) {
	if (!th)
		return -ESRCH;
	if (!trace_is_stopped(th))
		return -EINVAL;
	memcpy(th->user_ctx, data, sizeof(interrupt_frame));
	return 0;
}

static sysret trace_start(struct thread *th, enum trace_state ns, int signal) {
	if (!th)
		return -ESRCH;
	bool should_start = trace_is_stopped(th);
	bool in_signal = th->trace_state == TRACE_SIGNAL_DELIVERY_STOP;

	th->trace_state = ns;

	/* When you continue a traced thread, you may pass a signal that will
	 * be delivered to that thread. If the thread is stopped in signal
	 * delivery stop, it will live replace the recieved signal (or remove
	 * it).
	 * Otherwise, I just add the signal to the pending set. This may not
	 * technically be the correct behavior if there are already pending
	 * signals, but it sure beats longjmping directly to handle signal
	 * out of nowhere, which is the only alternative that comes to mind.
	 */
	if (in_signal) {
		th->trace_signal = signal;
	} else {
		if (signal)
			sigaddset(&th->sig_pending, signal);
	}

	if (ns == TRACE_SINGLESTEP) {
		th->user_ctx->flags |= TRAP_FLAG;
	} else {
		th->user_ctx->flags &= ~TRAP_FLAG;
	}

	if (should_start) {
		sched_wake(th);
	}
	return 0;
}

static sysret trace_detach(struct thread *th) {
	if (!th)
		return -ESRCH;
	list_remove(&th->trace_node);
	th->tracer = nullptr;
	return trace_start(th, TRACE_RUNNING, 0);
}

sysret sys_trace(enum trace_command cmd, pid_t pid, void *addr, void *data) {
	// Security: this currently permits anyone to trace anything. Eventually
	// this should be updated to have some more complex checking and policy
	// framework
	struct thread *th = thread_by_id(pid);
	int d_signal = (int)(intptr_t)data; // fun warnings

	switch (cmd) {
	case TR_TRACEME:
		return trace_traceme();
	case TR_ATTACH:
		return trace_attach(th);
	case TR_GETREGS:
		return trace_getregs(th, data);
	case TR_SETREGS:
		return trace_setregs(th, data);
	case TR_READMEM:
		return -ETODO;
	case TR_WRITEMEM:
		return -ETODO;
	case TR_SINGLESTEP:
		return trace_start(th, TRACE_SINGLESTEP, d_signal);
	case TR_SYSCALL:
		return trace_start(th, TRACE_SYSCALL, d_signal);
	case TR_CONT:
		return trace_start(th, TRACE_RUNNING, d_signal);
	case TR_DETACH:
		return trace_detach(th);
	}
	return -EINVAL;
}

static void wake_tracer_with(struct thread *tracee, int value) {
	struct process *tracer = tracee->tracer;
	if (!tracer)
		return;

	tracee->trace_report = value;

	spin_lock(&tracer->wait_lock);
	wq_wake_all_locked(&tracer->wait_wq);
	spin_unlock(&tracer->wait_lock);

	signal_send_proc(tracer, SIGCHLD);
}

void trace_syscall_entry(struct thread *tracee, int syscall) {
	if (tracee->trace_state == TRACE_RUNNING)
		return;

	int report = TRACE_SYSCALL_ENTRY | syscall;

	tracee->trace_state = TRACE_SYSCALL_ENTER_STOP;
	wake_tracer_with(tracee, report);

	sched_block();
}

void trace_syscall_exit(struct thread *tracee, int syscall) {
	if (tracee->trace_state == TRACE_RUNNING)
		return;

	int report = TRACE_SYSCALL_EXIT | syscall;

	tracee->trace_state = TRACE_SYSCALL_EXIT_STOP;
	wake_tracer_with(tracee, report);

	sched_block();
}

int trace_signal_delivery(int signal, sighandler_t handler) {
	struct thread *tracee = running_addr();
	if (!running_thread->tracer)
		return signal;
	int report = TRACE_SIGNAL | signal;

	tracee->trace_state = TRACE_SIGNAL_DELIVERY_STOP;
	wake_tracer_with(tracee, report);

	sched_block();

	return tracee->trace_signal;
}

void trace_report_trap(int interrupt) {
	assert(running_thread->tracer);

	struct thread *tracee = running_addr();
	int report = TRACE_TRAP | interrupt;

	tracee->trace_state = TRACE_TRAPPED;
	wake_tracer_with(tracee, report);

	sched_block();
}
