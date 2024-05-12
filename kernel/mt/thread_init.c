#include <ng/arch-2.h>
#include <ng/thread_transition.h>

[[noreturn]] void thread_entrypoint() {
	struct thread *th = running_addr();

	th->entry(th->entry_arg);
	UNREACHABLE();
}

void new_userspace_entry_exec(void *filename) {
	interrupt_frame *frame
		= (void *)(USER_STACK - 16 - sizeof(interrupt_frame));
	sysret err = sys_execve(frame, filename, nullptr, nullptr);
	assert(err == 0 && "BOOTSTRAP ERROR");

	frame->rax = 1;
	frame->r8 = 8;
	frame->r15 = 15;

	jump_to_frame(frame);
	UNREACHABLE();
}

void new_entry_frame(void *frame) {
	jump_to_frame(frame);
	UNREACHABLE();
}

void bootstrap_usermode(const char *init_filename) {
	dmgr_drop(&threads, 1);
	struct thread *th = new_thread();
	struct process *proc = new_process(th);

	th->entry = new_userspace_entry_exec;
	th->entry_arg = (char *)init_filename;
	th->cwd = resolve_path("/bin");

	proc->mmap_base = USER_MMAP_BASE;
	proc->vm_root = vmm_fork(proc, running_process);

	th->state = TS_RUNNING;

	thread_enqueue(th);
}

struct thread *new_thread() {
	struct thread *th = new_thread_slot();
	int new_tid = dmgr_insert(&threads, th);
	memset(th, 0, sizeof(struct thread));
	th->state = TS_PREINIT;

	list_init(&th->tracees);
	list_init(&th->process_threads);
	list_append(&all_threads, &th->all_threads);

	th->kstack = (char *)new_kernel_stack();
	th->kernel_ctx->__regs.sp = (uintptr_t)th->kstack - 8;
	th->kernel_ctx->__regs.bp = (uintptr_t)th->kstack - 8;
	th->kernel_ctx->__regs.ip = (uintptr_t)thread_entrypoint;

	th->tid = new_tid;
	th->irq_disable_depth = 1;
	th->magic = THREAD_MAGIC;
	th->tlsbase = 0;
	th->report_events = running_thread->report_events;
	// th->syscall_trace = true;

	log_event(EVENT_THREAD_NEW, "new thread: %i\n", new_tid);

	return th;
}

struct thread *kthread_create(void (*entry)(void *), void *arg) {
	DEBUG_PRINTF("new_kernel_thread(%p)\n", entry);

	struct thread *th = new_thread();

	th->entry = entry;
	th->entry_arg = arg;
	th->proc = &proc_zero;
	th->is_kthread = true;
	list_append(&proc_zero.threads, &th->process_threads);

	th->state = TS_STARTED;
	thread_enqueue(th);
	return th;
}

struct process *new_process(struct thread *th) {
	struct process *proc = new_process_slot();
	memset(proc, 0, sizeof(struct process));
	proc->magic = PROC_MAGIC;

	list_init(&proc->children);
	list_init(&proc->threads);

	proc->root = global_root_dentry;

	proc->pid = th->tid;
	proc->parent = running_process;
	th->proc = proc;

	list_append(&running_process->children, &proc->siblings);
	list_append(&proc->threads, &th->process_threads);

	return proc;
}

sysret sys_create(const char *executable) { return -ETODO; }

sysret sys_procstate(pid_t destination, enum procstate flags) {
	struct process *d_p = process_by_id(destination);
	if (!d_p)
		return -ESRCH;
	if (d_p == ZOMBIE)
		return -ESRCH;
	struct process *p = running_process;

	if (flags & PS_COPYFDS) {
		// clone_all_files_to(d_p)
	}

	if (flags & PS_SETRUN) {
		struct thread *th;
		th = container_of(
			struct thread, process_threads, list_head(&d_p->threads));
		th->state = TS_RUNNING;
		thread_enqueue(th);
	}

	return 0;
}
