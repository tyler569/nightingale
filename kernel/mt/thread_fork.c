#include <ng/thread_transition.h>

void thread_copy_flags_to_new(struct thread *new) {
	new->syscall_trace_children = running_thread->syscall_trace_children;
	new->in_signal = running_thread->in_signal;
	if (running_thread->syscall_trace_children) {
		new->syscall_trace = true;
	}
}

sysret sys_fork(struct interrupt_frame *r) {
	DEBUG_PRINTF("sys_fork(%p)\n", r);

	if (running_process->pid == 0)
		panic("Cannot fork() the kernel\n");

	struct thread *new_th = new_thread();
	struct process *new_proc = new_process(new_th);

	strncpy(new_proc->comm, running_process->comm, COMM_SIZE);
	new_proc->pgid = running_process->pgid;
	new_proc->uid = running_process->uid;
	new_proc->gid = running_process->gid;
	new_proc->mmap_base = running_process->mmap_base;
	new_proc->elf_metadata = clone_elf_md(running_process->elf_metadata);

	// copy files to child
	new_proc->fds = clone_all_files(running_process);
	// dmgr_clone(new_proc->fds, running_process->fds);

	new_th->rsp = running_thread->rsp;

	thread_copy_flags_to_new(new_th);

	new_th->proc = new_proc;
	new_th->cwd = running_thread->cwd;

	struct interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
	memcpy(frame, r, sizeof(interrupt_frame));
	FRAME_RETURN(frame) = 0;
	new_th->user_ctx = frame;
	new_th->user_ctx_valid = true;

	new_th->kernel_ctx->__regs.ip = (uintptr_t)0; // TODO
	new_th->kernel_ctx->__regs.sp = (uintptr_t)new_th->user_ctx;
	new_th->kernel_ctx->__regs.bp = (uintptr_t)new_th->user_ctx;

	new_proc->vm_root = vmm_fork(new_proc, running_process);
	new_th->state = TS_STARTED;
	new_th->irq_disable_depth = running_thread->irq_disable_depth;

	thread_enqueue(new_th);
	return new_proc->pid;
}

sysret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), void *new_stack,
	int flags, void *arg) {
	DEBUG_PRINTF(
		"sys_clone0(%p, %p, %p, %p, %i)\n", r, fn, new_stack, arg, flags);

	if (running_process->pid == 0) {
		panic("Cannot clone() the kernel - you want kthread_create\n");
	}

	struct thread *new_th = new_thread();

	list_append(&running_process->threads, &new_th->process_threads);

	thread_copy_flags_to_new(new_th);

	new_th->proc = running_process;
	new_th->cwd = running_thread->cwd;

	struct interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
	memcpy(frame, r, sizeof(interrupt_frame));
	FRAME_RETURN(frame) = 0;
	new_th->user_ctx = frame;
	new_th->user_ctx_valid = true;

	frame->rsp = (uintptr_t)new_stack;
	frame->rbp = (uintptr_t)new_stack;
	frame->rip = (uintptr_t)fn;

	new_th->kernel_ctx->__regs.ip = (uintptr_t)0; // TODO
	new_th->kernel_ctx->__regs.sp = (uintptr_t)new_th->user_ctx;
	new_th->kernel_ctx->__regs.bp = (uintptr_t)new_th->user_ctx;

	new_th->state = TS_STARTED;

	thread_enqueue(new_th);

	return new_th->tid;
}
