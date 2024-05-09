#include <ng/thread_transition.h>

LIST_HEAD(freeable_thread_queue);
struct thread *finalizer = nullptr;

void make_freeable(struct thread *defunct) {
	assert(defunct->state == TS_DEAD);
	assert(defunct->freeable.next == nullptr);
	DEBUG_PRINTF("freeable(%i)\n", defunct->tid);
	list_append(&freeable_thread_queue, &defunct->freeable);
	thread_enqueue(finalizer);
}

[[noreturn]] void finalizer_kthread(void *) {
	while (true) {
		struct thread *th;

		if (list_empty(&freeable_thread_queue)) {
			thread_block();
		} else {
			struct list_head *it = list_pop_front(&freeable_thread_queue);
			th = container_of(struct thread, freeable, it);
			free_kernel_stack(th);
			free_thread_slot(th);
		}
	}
}

void do_process_exit(int exit_status) {
	if (running_process->pid == 1)
		panic("attempted to kill init!");
	assert(list_empty(&running_process->threads));
	running_process->exit_status = exit_status + 1;

	struct process *init = process_by_id(1);
	if (!list_empty(&running_process->children)) {
		list_for_each (&running_process->children) {
			struct process *child = container_of(struct process, siblings, it);
			child->parent = init;
		}
		list_concat(&init->children, &running_process->children);
	}

	wake_waiting_parent_thread();
}

[[noreturn]] void do_thread_exit(int exit_status) {
	DEBUG_PRINTF("do_thread_exit(%i)\n", exit_status);
	assert(running_thread->state != TS_DEAD);

	// list_remove(&running_addr()->wait_node);
	list_remove(&running_addr()->trace_node);
	list_remove(&running_addr()->process_threads);
	list_remove(&running_addr()->all_threads);
	list_remove(&running_addr()->runnable);

	if (running_thread->wait_event) {
		drop_timer_event(running_addr()->wait_event);
	}

	if (running_thread->tid == running_process->pid) {
		running_process->exit_intention = exit_status + 1;
		dmgr_set(&threads, running_thread->tid, ZOMBIE);
	} else {
		dmgr_drop(&threads, running_thread->tid);
	}

	log_event(EVENT_THREAD_DIE, "die thread: %i\n", running_thread->tid);

	if (list_empty(&running_process->threads))
		do_process_exit(exit_status);

	running_thread->state = TS_DEAD;
	make_freeable(running_addr());
	thread_done_irqs_disabled();
}

[[noreturn]] sysret sys__exit(int exit_status) {
	kill_process(running_process, exit_status);
	UNREACHABLE();
}

[[noreturn]] sysret sys_exit_thread(int exit_status) {
	do_thread_exit(exit_status);
}

[[noreturn]] void kthread_exit() { do_thread_exit(0); }

void destroy_child_process(struct process *proc) {
	assert(proc != running_process);
	assert(proc->exit_status);
	void *child_thread = dmgr_get(&threads, proc->pid);
	assert(child_thread == ZOMBIE);
	dmgr_drop(&threads, proc->pid);

	// ONE OF THESE IS WRONG
	assert(list_empty(&proc->threads));
	list_remove(&proc->siblings);

	close_all_files(proc);

	vmm_destroy_tree(proc->vm_root);
	if (proc->elf_metadata)
		free(proc->elf_metadata);
	proc->elf_metadata = nullptr;
	free_process_slot(proc);
}

sysret sys_exit_group(int exit_status) {
	kill_process(running_process, exit_status);
	UNREACHABLE();
}

void kill_process(struct process *p, int reason) {
	struct thread *th, *tmp;

	if (list_empty(&p->threads))
		return;
	p->exit_intention = reason + 1;

	if (p == running_process)
		do_thread_exit(reason);
}

void kill_pid(pid_t pid) {
	struct process *p = process_by_id(pid);
	if (!p)
		return;
	if (p == ZOMBIE)
		return;
	kill_process(p, 0);
}

sysret sys_haltvm(int exit_code) { return -ENOSYS; }
