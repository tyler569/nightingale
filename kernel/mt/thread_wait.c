#include <ng/thread_transition.h>

void wake_waiting_parent_thread() {
	if (running_process->pid == 0)
		return;
	struct process *parent = running_process->parent;
	list_for_each (&parent->threads) {
		struct thread *parent_th
			= container_of(struct thread, process_threads, it);
		if (parent_th->state != TS_WAIT)
			continue;
		if (process_matches(parent_th->wait_request, running_process)) {
			parent_th->wait_result = running_process;
			parent_th->state = TS_RUNNING;
			signal_send_th(parent_th, SIGCHLD);
			return;
		}
	}

	// no one is listening, signal the tg leader
	struct thread *parent_th = process_thread(parent);
	signal_send_th(parent_th, SIGCHLD);
}

// If it finds a child process to destroy, find_dead_child returns with
// interrupts disabled. destroy_child_process will re-enable them.
struct process *find_dead_child(pid_t query) {
	if (list_empty(&running_process->children))
		return nullptr;
	list_for_each (&running_process->children) {
		struct process *child = container_of(struct process, siblings, it);
		if (!process_matches(query, child))
			continue;
		if (child->exit_status > 0)
			return child;
	}
	return nullptr;
}

struct thread *find_waiting_tracee(pid_t query) {
	if (list_empty(&running_addr()->tracees))
		return nullptr;
	list_for_each (&running_addr()->tracees) {
		struct thread *th = container_of(struct thread, trace_node, it);
		if (query != 0 && query != th->tid)
			continue;
		if (th->state == TS_TRWAIT)
			return th;
	}
	return nullptr;
}

void wait_for(pid_t pid) {
	running_thread->state = TS_WAIT;
	running_thread->wait_request = pid;
	running_thread->wait_result = 0;
	running_thread->wait_trace_result = 0;
}

void clear_wait() {
	running_thread->wait_request = 0;
	running_thread->wait_result = 0;
	running_thread->wait_trace_result = 0;
	running_thread->state = TS_RUNNING;
}

sysret sys_waitpid(pid_t pid, int *status, enum wait_options options) {
	DEBUG_PRINTF("[%i] waitpid(%i, xx, xx)\n", running_thread->tid, pid);

	int exit_code;
	int found_pid;

	wait_for(pid);

	struct process *child = find_dead_child(pid);
	if (child) {
		clear_wait();

		exit_code = child->exit_status - 1;
		found_pid = child->pid;
		log_event(EVENT_THREAD_REAP, "reap pid: %i\n", found_pid);
		destroy_child_process(child);

		if (status)
			*status = exit_code;
		return found_pid;
	}

	struct thread *trace_th = find_waiting_tracee(pid);
	if (trace_th) {
		clear_wait();

		if (status)
			*status = trace_th->trace_report;
		return trace_th->tid;
	}

	if (list_empty(&running_process->children)
		&& list_empty(&running_addr()->tracees)) {
		clear_wait();
		return -ECHILD;
	}

	if (options & WNOHANG)
		return 0;

	if (running_thread->state == TS_WAIT) {
		thread_block();
		// rescheduled when a wait() comes in
		// see wake_waiting_parent_thread();
		// and trace_wake_tracer_with();
	}
	if (running_thread->state == TS_WAIT)
		return -EINTR;

	struct process *p = running_thread->wait_result;
	struct thread *t = running_thread->wait_trace_result;
	clear_wait();

	if (p) {
		exit_code = p->exit_status - 1;
		found_pid = p->pid;
		log_event(EVENT_THREAD_REAP, "reap pid: %i\n", found_pid);
		destroy_child_process(p);

		if (status)
			*status = exit_code;
		return found_pid;
	}
	if (t) {
		if (status)
			*status = t->trace_report;
		return t->tid;
	}
	return -EINTR;
	UNREACHABLE();
}
