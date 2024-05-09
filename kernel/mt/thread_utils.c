#include <ng/thread_transition.h>

LIST_HEAD(all_threads);

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

extern char hhstack_top; // boot.asm

struct process proc_zero = {
	.pid = 0,
	.magic = PROC_MAGIC,
	.comm = "<nightingale>",
	.vm_root = -1, // this is reset in arch_init
	.parent = nullptr,
	.children = LIST_INIT(proc_zero.children),
	.threads = LIST_INIT(proc_zero.threads),
};

struct thread thread_zero = {
	.tid = 0,
	.magic = THREAD_MAGIC,
	.kstack = &hhstack_top,
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
	struct thread *idle_thread = new_thread();
	idle_thread->is_kthread = true;
	idle_thread->on_cpu = true;
	idle_thread->irq_disable_depth = 1;
	idle_thread->state = TS_RUNNING;
	idle_thread->proc = &proc_zero;
	list_append(&proc_zero.threads, &idle_thread->process_threads);

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

struct process *new_process_slot() { return malloc(sizeof(struct process)); }

struct thread *new_thread_slot() { return malloc(sizeof(struct thread)); }

void free_process_slot(struct process *defunct) { free(defunct); }

void free_thread_slot(struct thread *defunct) {
	assert(defunct->state == TS_DEAD);
	free(defunct);
}

struct thread *thread_by_id(pid_t tid) { return dmgr_get(&threads, tid); }

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

sysret sys_getpid() { return running_process->pid; }

sysret sys_gettid() { return running_thread->tid; }

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
	struct mm_region *slot = nullptr, *test;
	for (int i = 0; i < NREGIONS; i++) {
		test = &running_process->mm_regions[i];
		if (test->base == 0) {
			slot = test;
			break;
		}
	}

	if (!slot)
		return false;
	slot->base = base;
	slot->top = top;
	slot->vnode = 0;

	vmm_create_unbacked_range(base, top - base, PAGE_WRITEABLE | PAGE_USERMODE);
	return true;
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
