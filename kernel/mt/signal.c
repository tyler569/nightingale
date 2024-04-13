#include <assert.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/event_log.h>
#include <ng/memmap.h>
#include <ng/signal.h>
#include <ng/thread.h>
#include <ng/x86/interrupt.h>

#define SIGSTACK_LEN 2048

sysret sys_sigaction(int sig, sighandler_t handler, int flags) {
	if (sig < 0 || sig > 32)
		return -EINVAL;

	// Flags is intended for things like specifying that the signal
	// handler is interested in an additional parameter with more
	// information about the signal. See siginfo_t on Linux.
	if (flags)
		return -ETODO;

	running_thread->sig_handlers[sig] = handler;
	return 0;
}

sysret sys_sigprocmask(int op, const sigset_t *new, sigset_t *old) {
	sigset_t old_mask = running_thread->sig_mask;

	switch (op) {
	case SIG_BLOCK:
		running_thread->sig_mask |= *new;
		break;
	case SIG_UNBLOCK:
		running_thread->sig_mask &= ~(*new);
		break;
	case SIG_SETMASK:
		running_thread->sig_mask = *new;
		break;
	default:
		return -EINVAL;
	}

	if (old)
		*old = old_mask;
	return 0;
}

[[noreturn]] sysret sys_sigreturn(int code) {
	struct thread *th = running_addr();

	set_kernel_stack(th->kstack);
	th->in_signal = false;

	th->state = th->nonsig_state;

	if (th->state == TS_RUNNING) {
		longjmp(th->kernel_ctx, 2);
	} else {
		struct thread *next = thread_sched();
		thread_switch_no_save(next);
	}
}

int signal_send_th(struct thread *th, int signal) {
	log_event(EVENT_SIGNAL, "send signal %i from %i to %i\n", signal,
		running_thread->tid, th->tid);
	sigaddset(&th->sig_pending, signal);
	thread_enqueue(th);

	return 0;
}

int signal_send(pid_t pid, int signal) {
	if (pid < 0)
		return -ETODO;
	if (pid == 0)
		return -EPERM;

	struct thread *th = thread_by_id(pid);
	if (!th)
		return -ESRCH;
	if (th->is_kthread)
		return -EPERM;

	return signal_send_th(th, signal);
}

int signal_send_pgid(pid_t pgid, int signal) {
	list_for_each_safe (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		struct process *p = th->proc;
		if (th->tid != p->pid)
			continue;
		if (pgid != p->pgid)
			continue;
		signal_send_th(th, signal);
	}
	return 0;
}

sysret sys_kill(pid_t pid, int sig) { return signal_send(pid, sig); }

bool signal_is_actionable(struct thread *th, int signal) {
	if (sigismember(&th->sig_mask, signal))
		return false;
	return sigismember(&th->sig_pending, signal);
}

int handle_pending_signals() {
	struct thread *th = running_addr();

	for (int signal = 0; signal < 32; signal++) {
		if (!signal_is_actionable(th, signal))
			continue;

		sigdelset(&th->sig_pending, signal);
		handle_signal(signal, th->sig_handlers[signal]);
	}

	return 0;
}

void signal_self(int signal) {
	handle_signal(signal, running_thread->sig_handlers[signal]);
}

void handle_signal(int signal, sighandler_t handler) {
	if (signal == SIGKILL)
		kill_process(running_process, signal + 128);

	// the tracer can change what signal is delivered to the traced thread.
	signal = trace_signal_delivery(signal, handler);
	if (!signal)
		return;

	if (signal == SIGSTOP) {
		running_thread->stopped = true;
		return;
	}
	if (signal == SIGCONT) {
		running_thread->stopped = false;
		return;
	}
	if (handler == SIG_IGN)
		return;
	if (handler == SIG_DFL) {
		switch (signal) {
		case SIGCHLD:
		case SIGINFO:
		case SIGURG:
		case SIGWINCH:
			return;
		default:
			kill_process(running_process, signal + 128);
		}
	}
	do_signal_call(signal, handler);
}

static char static_signal_stack[SIGSTACK_LEN];
static char *sigstack = static_signal_stack + SIGSTACK_LEN;

void do_signal_call(int sig, sighandler_t handler) {
	running_thread->nonsig_state = running_thread->state;
	running_thread->state = TS_RUNNING;
	running_thread->in_signal = true;

	uintptr_t old_sp = running_thread->user_ctx->user_sp;

	uintptr_t new_sp = ROUND_DOWN(old_sp - 128, 16);

	uintptr_t *pnew_sp = (uintptr_t *)new_sp;
	pnew_sp[0] = 0; // rbp + 8
	pnew_sp[1] = 0; // rbp

	set_kernel_stack(sigstack);

	jmp_to_userspace((uintptr_t)handler, new_sp, sig);

	assert(0);
}
