#include <elf.h>
#include <ng/arch.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/fs/proc.h>
#include <ng/memmap.h>
#include <ng/mman.h>
#include <ng/panic.h>
#include <ng/signal.h>
#include <ng/string.h>
#include <ng/sync.h>
#include <ng/syscalls.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <ng/vmo.h>
#include <ng/x86/interrupt.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

[[noreturn]] void finalizer_kthread(void *);
void thread_timer(void *);
void wake_waiting_parent_thread();

void thread_set_running(struct thread *th);
void thread_yield();
void thread_block();
void thread_block_irqs_disabled();
[[noreturn]] void thread_done();
[[noreturn]] void thread_done_irqs_disabled();

enum in_out { SCH_IN, SCH_OUT };

#define NCPUS 32
#define THREAD_STACK_SIZE 0x2000
#define THREAD_TIME milliseconds(5)
#define ZOMBIE (void *)2
#define thread_idle (this_cpu->idle)

LIST_DEFINE(all_threads);
LIST_DEFINE(freeable_thread_queue);
struct thread *finalizer = nullptr;
LIST_DEFINE(runnable_thread_queue);
spinlock_t runnable_lock;
struct dmgr threads;

const char *thread_states[] = {
	[TS_INVALID] = "TS_INVALID",
	[TS_PREINIT] = "TS_PREINIT",
	[TS_STARTED] = "TS_STARTED",
	[TS_RUNNING] = "TS_RUNNING",
	[TS_BLOCKED] = "TS_BLOCKED",
	[TS_WAIT] = "TS_WAIT",
	[TS_IOWAIT] = "TS_IOWAIT",
	[TS_TRWAIT] = "TS_TRWAIT",
	[TS_SLEEP] = "TS_SLEEP",
	[TS_DEAD] = "TS_DEAD",
};

enum thread_kind {
	THREAD_KIND_KERNEL,
	THREAD_KIND_USER,
};

enum proc_mode {
	PROC_MODE_NEW,
	PROC_MODE_EXISTING,
};

struct thread_spawn {
	enum thread_kind kind;
	enum proc_mode proc_mode;
	struct process *proc;

	void (*kentry)(void *);
	void *karg;

	const struct interrupt_frame *parent_frame;
	void *user_ip;
	void *user_sp;

	struct dentry *cwd;
	enum thread_state start_state;
	bool enqueue;
	bool set_on_cpu;
};

static void thread_attach_to_process(struct thread *th, struct process *proc) {
	th->proc = proc;
	list_append(&proc->threads, &th->process_threads);
}

static void thread_init_user_ctx(struct thread *th,
	const struct interrupt_frame *frame, void *user_ip, void *user_sp) {
	interrupt_frame *new_frame = (interrupt_frame *)th->kstack - 1;

	if (frame) {
		memcpy(new_frame, frame, sizeof(*new_frame));
	} else {
		memset(new_frame, 0, sizeof(*new_frame));
	}

	if (user_ip)
		new_frame->ip = (uintptr_t)user_ip;
	if (user_sp) {
		new_frame->user_sp = (uintptr_t)user_sp;
		new_frame->bp = (uintptr_t)user_sp;
	}

	th->user_ctx = new_frame;
	th->user_ctx_valid = true;

	th->kernel_ctx->__regs.ip = (uintptr_t)return_from_interrupt;
	th->kernel_ctx->__regs.sp = (uintptr_t)new_frame;
	th->kernel_ctx->__regs.bp = (uintptr_t)new_frame;
}

static struct thread *thread_spawn(
	const struct thread_spawn *sp, struct process **out_proc) {
	struct thread *th = new_thread();
	struct process *proc = nullptr;

	if (sp->kind == THREAD_KIND_KERNEL)
		th->is_kthread = true;

	if (sp->proc_mode == PROC_MODE_NEW) {
		proc = new_process(th);
	} else {
		assert(sp->proc);
		proc = sp->proc;
		thread_attach_to_process(th, proc);
	}

	if (sp->kentry) {
		th->entry = sp->kentry;
		th->entry_arg = sp->karg;
	}

	if (sp->cwd)
		th->cwd = sp->cwd;

	if (sp->parent_frame || sp->user_ip || sp->user_sp)
		thread_init_user_ctx(th, sp->parent_frame, sp->user_ip, sp->user_sp);

	if (sp->start_state)
		th->state = sp->start_state;

	if (sp->set_on_cpu)
		th->on_cpu = true;

	if (sp->enqueue)
		thread_enqueue(th);

	if (out_proc)
		*out_proc = proc;

	return th;
}

struct process proc_zero = {
	.pid = 0,
	.magic = PROC_MAGIC,
	.comm = "<nightingale>",
	.vm_root = -1, // this is reset in arch_init
	.parent = nullptr,
	.children = LIST_INIT(proc_zero.children),
	.threads = LIST_INIT(proc_zero.threads),
	.vmas = LIST_INIT(proc_zero.vmas),
};

// FIXME temporary, either use the limine stack or do something sensible.
static constexpr size_t th_0_stack_size = 8192;
static char th_0_stack[th_0_stack_size];

struct thread thread_zero = {
	.tid = 0,
	.magic = THREAD_MAGIC,
	.kstack = th_0_stack + th_0_stack_size,
	.state = TS_RUNNING,
	.is_kthread = true,
	.on_cpu = true,
	.irq_disable_depth = 1,
	.proc = &proc_zero,
};

struct cpu cpu_zero = {
	.self = &cpu_zero,
	.running = &thread_zero,
	.idle = &thread_zero,
};

struct cpu *thread_cpus[NCPUS] = { &cpu_zero };

void new_cpu(int n) {
	struct cpu *new_cpu = malloc(sizeof(struct cpu));
	struct thread_spawn sp = {
		.kind = THREAD_KIND_KERNEL,
		.proc_mode = PROC_MODE_EXISTING,
		.proc = &proc_zero,
		.start_state = TS_RUNNING,
		.enqueue = false,
		.set_on_cpu = true,
	};
	struct thread *idle_thread = thread_spawn(&sp, nullptr);
	idle_thread->irq_disable_depth = 1;

	new_cpu->self = new_cpu;
	new_cpu->idle = idle_thread;
	new_cpu->running = idle_thread;

	thread_cpus[n] = new_cpu;
}

void threads_init() {
	DEBUG_PRINTF("init_threads()\n");

	// spin_init(&runnable_lock);
	// mutex_init(&process_lock);
	proc_zero.root = global_root_dentry;

	dmgr_insert(&threads, &thread_zero);
	dmgr_insert(&threads, (void *)1); // save 1 for init

	list_append(&all_threads, &thread_zero.all_threads);
	list_append(&proc_zero.threads, &thread_zero.process_threads);

	finalizer = kthread_create(finalizer_kthread, nullptr);
	insert_timer_event(milliseconds(10), thread_timer, nullptr);
}

struct process *new_process_slot() {
	return malloc(sizeof(struct process));
}

struct thread *new_thread_slot() {
	return malloc(sizeof(struct thread));
}

void free_process_slot(struct process *defunct) {
	free(defunct);
}

void free_thread_slot(struct thread *defunct) {
	assert(defunct->state == TS_DEAD);
	free(defunct);
}

struct thread *thread_by_id(pid_t tid) {
	return dmgr_get(&threads, tid);
}

struct process *process_by_id(pid_t pid) {
	struct thread *th = thread_by_id(pid);
	if (th == nullptr)
		return nullptr;
	if (th == ZOMBIE)
		return ZOMBIE;
	return th->proc;
}

struct thread *process_thread(struct process *p) {
	return container_of(struct thread, process_threads, list_head(&p->threads));
}

void *new_kernel_stack() {
	char *new_stack = vmm_reserve(THREAD_STACK_SIZE);
	// touch the pages so they exist before we swap to this stack
	memset(new_stack, 0, THREAD_STACK_SIZE);
	void *stack_top = new_stack + THREAD_STACK_SIZE;
	return stack_top;
}

void free_kernel_stack(struct thread *th) {
	vmm_unmap_range(
		((uintptr_t)th->kstack) - THREAD_STACK_SIZE, THREAD_STACK_SIZE);
}

int process_matches(pid_t wait_arg, struct process *proc) {
	if (wait_arg == 0) {
		return 1;
	} else if (wait_arg > 0) {
		return wait_arg == proc->pid;
	} else if (wait_arg == -1) {
		return true;
	} else if (wait_arg < 0) {
		return -wait_arg == proc->pgid;
	}
	return 0;
}

sysret sys_getpid() {
	return running_process->pid;
}

sysret sys_gettid() {
	return running_thread->tid;
}

sysret sys_syscall_trace(pid_t tid, int state) {
	struct thread *th;
	if (tid == 0) {
		th = running_addr();
	} else {
		th = thread_by_id(tid);
	}
	if (!th)
		return -ESRCH;

	if (state == 0) {
		th->syscall_trace = false;
		th->syscall_trace_children = false;
	}
	if (state & 1)
		th->syscall_trace = true;
	if (state & 2)
		th->syscall_trace_children = true;

	return state;
}

sysret sys_setpgid(int pid, int pgid) {
	struct process *proc;
	if (pid == 0) {
		proc = running_process;
	} else {
		proc = process_by_id(pid);
	}

	if (!proc)
		return -ESRCH;
	if (proc == ZOMBIE)
		return -ESRCH;

	proc->pgid = pgid;
	return 0;
}

bool user_map(virt_addr_t base, virt_addr_t top) {
	struct vmo *vmo = vmo_new_anon(top - base);
	if (!vmo)
		return false;
	int ret = vma_map(running_process, base, top - base,
		PROT_READ | PROT_WRITE, MAP_PRIVATE, vmo, 0);
	vmo_unref(vmo);
	return ret == 0;
}

void print_cpu_info() {
	printf(
		"running thread [%i:%i]\n", running_thread->tid, running_process->pid);
}

void print_thread(struct thread *th) {
	char *status;
	switch (th->state) {
	default:
		status = "?";
		break;
	}

	printf("  t: %i %s%s%s\n", th->tid, "", status, " TODO");
}

void print_process(void *p) {
	struct process *proc = p;

	if (proc->exit_status <= 0) {
		printf("pid %i: %s\n", proc->pid, proc->comm);
	} else {
		printf("pid %i: %s (defunct: %i)\n", proc->pid, proc->comm,
			proc->exit_status);
	}

	list_for_each (&proc->threads) {
		struct thread *th = container_of(struct thread, process_threads, it);
		print_thread(th);
	}
}

sysret sys_top(int show_threads) {
	list_for_each (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		printf("  %i:%i '%s'\n", th->proc->pid, th->tid, th->proc->comm);
	}
	return 0;
}

sysret sys_settls(void *tlsbase) {
	running_thread->tlsbase = tlsbase;
	set_tls_base(tlsbase);
	return 0;
}

sysret sys_report_events(long event_mask) {
	running_thread->report_events = event_mask;
	return 0;
}

[[noreturn]] void thread_entrypoint() {
	struct thread *th = running_addr();

	th->entry(th->entry_arg);
	UNREACHABLE();
}

void new_userspace_entry(void *filename) {
	interrupt_frame *frame
		= (void *)(USER_STACK - 16 - sizeof(interrupt_frame));
	sysret err = sys_execve(frame, filename, nullptr, nullptr);
	assert(err == 0 && "BOOTSTRAP ERROR");

	asm volatile("mov %0, %%rsp \n\t"
				 "jmp return_from_interrupt \n\t"
		:
		: "rm"(frame));

	// jmp_to_userspace(frame->ip, frame->user_sp, 0, 0);
	UNREACHABLE();
}

void bootstrap_usermode(const char *init_filename) {
	dmgr_drop(&threads, 1);
	struct process *proc = nullptr;
	struct thread_spawn sp = {
		.kind = THREAD_KIND_USER,
		.proc_mode = PROC_MODE_NEW,
		.kentry = new_userspace_entry,
		.karg = (void *)init_filename,
		.cwd = resolve_path("/bin"),
		.start_state = TS_RUNNING,
		.enqueue = false,
	};
	struct thread *th = thread_spawn(&sp, &proc);

	proc->mmap_base = USER_MMAP_BASE;
	proc->vm_root = vmm_fork(proc, running_process);

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

	struct thread_spawn sp = {
		.kind = THREAD_KIND_KERNEL,
		.proc_mode = PROC_MODE_EXISTING,
		.proc = &proc_zero,
		.kentry = entry,
		.karg = arg,
		.start_state = TS_STARTED,
		.enqueue = true,
	};

	return thread_spawn(&sp, nullptr);
}

struct process *new_process(struct thread *th) {
	struct process *proc = new_process_slot();
	memset(proc, 0, sizeof(struct process));
	proc->magic = PROC_MAGIC;

	list_init(&proc->children);
	list_init(&proc->threads);
	list_init(&proc->vmas);

	proc->root = global_root_dentry;

	proc->pid = th->tid;
	proc->parent = running_process;
	th->proc = proc;

	list_append(&running_process->children, &proc->siblings);
	list_append(&proc->threads, &th->process_threads);

	return proc;
}

sysret sys_create(const char *executable) {
	return -ETODO; // not working with fs2
	struct process *proc = nullptr;
	struct thread_spawn sp = {
		.kind = THREAD_KIND_USER,
		.proc_mode = PROC_MODE_NEW,
		.kentry = new_userspace_entry,
		.karg = (void *)executable,
		.cwd = resolve_path("/bin"),
		.enqueue = false,
	};
	(void)thread_spawn(&sp, &proc);

	proc->mmap_base = USER_MMAP_BASE;
	proc->vm_root = vmm_fork(proc, running_process);
	proc->parent = process_by_id(1);

	return proc->pid;
}

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

	struct process *new_proc = nullptr;
	struct thread_spawn sp = {
		.kind = THREAD_KIND_USER,
		.proc_mode = PROC_MODE_NEW,
		.parent_frame = r,
		.start_state = TS_STARTED,
		.enqueue = false,
	};
	struct thread *new_th = thread_spawn(&sp, &new_proc);

	strncpy(new_proc->comm, running_process->comm, COMM_SIZE);
	new_proc->pgid = running_process->pgid;
	new_proc->uid = running_process->uid;
	new_proc->gid = running_process->gid;
	new_proc->mmap_base = running_process->mmap_base;
	new_proc->elf_metadata = clone_elf_md(running_process->elf_metadata);

	new_proc->fds = clone_all_files(running_process);

	new_th->user_sp = running_thread->user_sp;

	thread_copy_flags_to_new(new_th);

	new_th->cwd = running_thread->cwd;

	struct interrupt_frame *frame = new_th->user_ctx;
	FRAME_RETURN(frame) = 0;

	new_proc->vm_root = vmm_fork(new_proc, running_process);
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

	struct thread_spawn sp = {
		.kind = THREAD_KIND_USER,
		.proc_mode = PROC_MODE_EXISTING,
		.proc = running_process,
		.parent_frame = r,
		.user_ip = fn,
		.user_sp = new_stack,
		.start_state = TS_STARTED,
		.enqueue = false,
	};
	struct thread *new_th = thread_spawn(&sp, nullptr);

	thread_copy_flags_to_new(new_th);

	new_th->cwd = running_thread->cwd;

	struct interrupt_frame *frame = new_th->user_ctx;
	FRAME_RETURN(frame) = 0;

	thread_enqueue(new_th);

	return new_th->tid;
}

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
			list_node *it = list_pop_front(&freeable_thread_queue);
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

[[noreturn]] void kthread_exit() {
	do_thread_exit(0);
}

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

	vma_drop_list(proc);
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

void thread_block_irqs_disabled() {
	thread_block();
}

[[noreturn]] void thread_done() {
	struct thread *to = thread_sched();
	thread_switch(to, running_addr());
	UNREACHABLE();
}

[[noreturn]] void thread_done_irqs_disabled() {
	thread_done();
}

bool needs_fpu(struct thread *th) {
	return th->proc->pid != 0;
}

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

void unsleep_thread(struct thread *t) {
	t->wait_event = nullptr;
	t->state = TS_RUNNING;
	thread_enqueue(t);
}

void unsleep_thread_callback(void *t) {
	unsleep_thread(t);
}

void sleep_thread(int ms) {
	assert(running_thread->tid != 0);
	int ticks = milliseconds(ms);
	struct timer_event *te
		= insert_timer_event(ticks, unsleep_thread_callback, running_addr());
	running_thread->wait_event = te;
	running_thread->state = TS_SLEEP;
	thread_block();
}

sysret sys_sleepms(int ms) {
	sleep_thread(ms);
	return 0;
}

void thread_timer(void *) {
	insert_timer_event(THREAD_TIME, thread_timer, nullptr);
	thread_yield();
}

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
