#include <ng/thread_transition.h>

LIST_DEFINE(runnable_thread_queue);
spinlock_t runnable_lock;

bool enqueue_checks(struct thread *th) {
	if (th->tid == 0)
		return false;
	// if (th->trace_state == TRACE_STOPPED)  return false;
	// I hope the above is covered by TRWAIT, but we'll see
	if (th->queued)
		return false;
	assert(th->proc->pid > -1);
	assert(th->magic == THREAD_MAGIC);
	// if (th->state != TS_RUNNING && th->state != TS_STARTED) {
	//     printf("fatal: thread %i state is %s\n", th->tid,
	//             thread_states[th->state]);
	// }
	// assert(th->state == TS_RUNNING || th->state == TS_STARTED);
	th->queued = true;
	return true;
}

void thread_enqueue(struct thread *th) {
	spin_lock(&runnable_lock);
	if (enqueue_checks(th)) {
		list_append(&runnable_thread_queue, &th->runnable);
	}
	spin_unlock(&runnable_lock);
}

void thread_enqueue_at_front(struct thread *th) {
	spin_lock(&runnable_lock);
	if (enqueue_checks(th)) {
		list_prepend(&runnable_thread_queue, &th->runnable);
	}
	spin_unlock(&runnable_lock);
}

struct thread *next_runnable_thread() {
	if (list_empty(&runnable_thread_queue))
		return nullptr;
	struct thread *rt;
	spin_lock(&runnable_lock);
	list_node *it = list_pop_front(&runnable_thread_queue);
	rt = container_of(struct thread, runnable, it);
	spin_unlock(&runnable_lock);
	rt->queued = false;
	return rt;
}

/*
 * Choose the next thread to run.
 *
 * This procedure disables interrupts and expects you to re-enable them
 * when you're done doing whatever you need to with this information.
 *
 * It does dequeue the thread from the runnable queue, so consider that
 * if you don't actually plan on running it.
 */
struct thread *thread_sched() {
	struct thread *to;
	to = next_runnable_thread();

	if (!to)
		to = thread_idle;
	assert(to->magic == THREAD_MAGIC);
	// assert(to->state == TS_RUNNING || to->state == TS_STARTED);
	return to;
}

void thread_set_running(struct thread *th) {
	this_cpu->running = th;
	th->on_cpu = true;
	if (th->state == TS_STARTED)
		th->state = TS_RUNNING;
}

void thread_yield() {
	struct thread *to = thread_sched();
	if (to == thread_idle) {
		return;
	}

	if (running_thread->state == TS_RUNNING)
		thread_enqueue(running_addr());
	thread_switch(to, running_addr());
}

void thread_block() {
	struct thread *to = thread_sched();
	thread_switch(to, running_addr());
}

void thread_block_irqs_disabled() { thread_block(); }

[[noreturn]] void thread_done() {
	struct thread *to = thread_sched();
	thread_switch(to, running_addr());
	UNREACHABLE();
}

[[noreturn]] void thread_done_irqs_disabled() { thread_done(); }

bool needs_fpu(struct thread *th) { return th->proc->pid != 0; }

bool change_vm(struct thread *new, struct thread *old) {
	return new->proc->vm_root != old->proc->vm_root && !new->is_kthread;
}

void account_thread(struct thread *th, enum in_out st) {
	uint64_t tick_time = kernel_timer;
	uint64_t tsc_time = rdtsc();

	if (st == SCH_IN) {
		th->n_scheduled += 1;
		th->last_scheduled = tick_time;
		th->tsc_scheduled = tsc_time;
	} else if (th->last_scheduled) {
		th->time_ran += tick_time - th->last_scheduled;
		th->tsc_ran += tsc_time - th->tsc_scheduled;
	}
}

void handle_stopped_condition() {
	while (running_thread->stopped)
		thread_block();
}

void handle_killed_condition() {
	if (running_thread->state == TS_DEAD)
		return;
	if (running_process->exit_intention) {
		do_thread_exit(running_process->exit_intention - 1);
	}
}

void thread_switch(
	struct thread *restrict new_thread, struct thread *restrict old_thread) {
	set_kernel_stack(new_thread->kstack);

	if (needs_fpu(old_thread))
		arch_thread_context_save(old_thread);
	if (needs_fpu(new_thread))
		arch_thread_context_restore(new_thread);
	if (change_vm(new_thread, old_thread))
		set_vm_root(new_thread->proc->vm_root);
	thread_set_running(new_thread);

	DEBUG_PRINTF("[%i:%i] -> [%i:%i]\n", old_thread->proc->pid, old_thread->tid,
		new_thread->proc->pid, new_thread->tid);

	log_event(EVENT_THREAD_SWITCH,
		"switch thread [%i:%i] (state %i) -> [%i:%i] (state %i)\n",
		old_thread->proc->pid, old_thread->tid, old_thread->state,
		new_thread->proc->pid, new_thread->tid, new_thread->state);

	if (setjmp(old_thread->kernel_ctx)) {
		old_thread->on_cpu = false;
		if (new_thread->tlsbase)
			set_tls_base(new_thread->tlsbase);
		if (!old_thread->is_kthread)
			old_thread->irq_disable_depth += 1;
		if (!running_thread->is_kthread) {
			handle_killed_condition();
			handle_pending_signals();
			handle_stopped_condition();
		}
		if (running_thread->state != TS_RUNNING)
			thread_block();
		if (!running_thread->is_kthread)
			enable_irqs();
		return;
	}
	account_thread(old_thread, SCH_OUT);
	account_thread(new_thread, SCH_IN);
	longjmp(new_thread->kernel_ctx, 1);
}

[[noreturn]] void thread_switch_no_save(struct thread *new_thread) {
	set_kernel_stack(new_thread->kstack);

	if (needs_fpu(new_thread))
		arch_thread_context_restore(new_thread);
	set_vm_root(new_thread->proc->vm_root);
	thread_set_running(new_thread);
	longjmp(new_thread->kernel_ctx, 1);
}

sysret sys_yield() {
	thread_yield();
	return 0;
}
