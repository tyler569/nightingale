#include <elf.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/fs/proc.h>
#include <ng/memmap.h>
#include <ng/mt/process.h>
#include <ng/mt/thread.h>
#include <ng/panic.h>
#include <ng/signal.h>
#include <ng/string.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <ng/x86/interrupt.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/wait.h>

#define THREAD_STACK_SIZE 0x2000

nx::list<thread, &thread::all_threads> all_threads;
nx::list<thread, &thread::runnable> runnable_thread_queue;
nx::list<thread, &thread::freeable> freeable_thread_queue;
nx::vector<thread *> threads;
spinlock_t runnable_lock;
thread *finalizer = nullptr;

extern tar_header *initfs;

#define THREAD_TIME milliseconds(5)
#define ZOMBIE (void *)2

_Noreturn static void finalizer_kthread(void *);
static void thread_timer(void *);
static void handle_killed_condition();
static void handle_stopped_condition();
void proc_threads(file *ofd, void *);
void proc_threads_detail(file *ofd, void *);
void proc_zombies(file *ofd, void *);
void proc_cpus(file *file, void *);
void thread_done_irqs_disabled();

extern "C" {
void make_proc_directory(thread *thread);
void destroy_proc_directory(dentry *proc_dir);
}

static thread *new_thread();

extern char hhstack_top; // boot.asm

process proc_zero = {
    .comm = "<nightingale>",
};

thread thread_zero(&proc_zero);

cpu cpu_zero = {
    .self = &cpu_zero,
    .idle = &thread_zero,
    .running = &thread_zero,
};

cpu *thread_cpus[NCPUS] = { &cpu_zero };

#define thread_idle (this_cpu->idle)

void new_cpu(int n)
{
    cpu *new_cpu = new cpu;
    thread *idle_thread = new_thread();
    idle_thread->flags = static_cast<thread_flags>(TF_IS_KTHREAD | TF_ON_CPU);
    idle_thread->irq_disable_depth = 1;
    idle_thread->state = TS_RUNNING;
    idle_thread->proc = thread_cpus[0]->idle->proc;
    idle_thread->proc->threads.push_back(*idle_thread);

    new_cpu->self = new_cpu;
    new_cpu->idle = idle_thread;
    new_cpu->running = idle_thread;

    thread_cpus[n] = new_cpu;
}

void threads_init()
{
    thread_zero.kstack = &hhstack_top;
    thread_zero.state = TS_RUNNING;
    thread_zero.flags = static_cast<thread_flags>(TF_IS_KTHREAD | TF_ON_CPU);
    thread_zero.irq_disable_depth = 1;
    thread_zero.proc = &proc_zero;

    proc_zero.root = global_root_dentry;

    threads.push_back(&thread_zero);
    threads.push_back((thread *)1); // save 1 for init

    all_threads.push_back(thread_zero);
    proc_zero.threads.push_back(thread_zero);

    make_proc_file("threads", proc_threads, nullptr);
    make_proc_file("threads2", proc_threads_detail, nullptr);
    make_proc_file("zombies", proc_zombies, nullptr);
    make_proc_file("thread_cpus", proc_cpus, nullptr);

    finalizer = kthread_create(finalizer_kthread, nullptr);
    insert_timer_event(milliseconds(10), thread_timer, nullptr);
}

static process *new_process_slot() { return new process(); }

static thread *new_thread_slot() { return new thread(); }

static void free_process_slot(process *defunct) { delete defunct; }

static void free_thread_slot(thread *defunct)
{
    assert(defunct->state == TS_DEAD);
    delete defunct;
}

thread *thread_by_id(pid_t tid)
{
    thread *th = threads[tid];
    if ((void *)th == ZOMBIE)
        return nullptr;
    return th;
}

process *process_by_id(pid_t pid)
{
    thread *th = thread_by_id(pid);
    if (th == nullptr)
        return nullptr;
    if ((void *)th == ZOMBIE)
        return (process *)ZOMBIE;
    return th->proc;
}

static void make_freeable(thread *defunct)
{
    assert(defunct->state == TS_DEAD);
    assert(defunct->freeable.is_null());
    DEBUG_PRINTF("freeable(%i)\n", defunct->tid);
    freeable_thread_queue.push_back(*defunct);
    thread_enqueue(finalizer);
}

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

static bool enqueue_checks(thread *th)
{
    DEBUG_PRINTF("enqueuing thread %i:%i\n", th->tid, th->proc->pid);
    if (th->tid == 0)
        return false;
    // if (th->trace_state == TRACE_STOPPED)  return false;
    // I hope the above is covered by TRWAIT, but we'll see
    if (th->flags & TF_QUEUED)
        return false;
    assert(th->proc->pid > -1);
    assert(th->magic == thread_magic);
    // if (th->state != TS_RUNNING && th->state != TS_STARTED) {
    //     printf("fatal: thread %i state is %s\n", th->tid,
    //             thread_states[th->state]);
    // }
    // assert(th->state == TS_RUNNING || th->state == TS_STARTED);
    th->add_flag(TF_QUEUED);
    return true;
}

void thread_enqueue(thread *th)
{
    spin_lock(&runnable_lock);
    if (enqueue_checks(th)) {
        runnable_thread_queue.push_back(*th);
    }
    spin_unlock(&runnable_lock);
}

void thread_enqueue_at_front(thread *th)
{
    spin_lock(&runnable_lock);
    if (enqueue_checks(th)) {
        runnable_thread_queue.push_front(*th);
    }
    spin_unlock(&runnable_lock);
}

// portability!
static void fxsave(fp_ctx *fpctx)
{
    // printf("called fxsave with %p\n", fpctx);
#ifdef __x86_64__
    asm volatile("fxsaveq %0" : : "m"(*fpctx));
#endif
}

static void fxrstor(fp_ctx *fpctx)
{
    // printf("called fxrstor with %p\n", fpctx);
#ifdef __x86_64__
    asm volatile("fxrstorq %0" : "=m"(*fpctx));
#endif
}

static thread *next_runnable_thread()
{
    if (runnable_thread_queue.empty())
        return nullptr;
    spin_lock(&runnable_lock);
    thread &rt = runnable_thread_queue.front();
    runnable_thread_queue.remove(rt);
    spin_unlock(&runnable_lock);
    rt.flags = static_cast<thread_flags>(rt.flags & ~TF_QUEUED);
    return &rt;
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
thread *thread_sched()
{
    thread *to;
    to = next_runnable_thread();

    if (!to)
        to = thread_idle;
    assert(to->magic == thread_magic);
    // assert(to->state == TS_RUNNING || to->state == TS_STARTED);
    return to;
}

static void thread_set_running(thread *th)
{
    this_cpu->running = th;
    th->flags = static_cast<thread_flags>(th->flags | TF_ON_CPU);
    if (th->state == TS_STARTED)
        th->state = TS_RUNNING;
}

void thread_yield()
{
    thread *to = thread_sched();
    if (to == thread_idle) {
        return;
    }

    if (running_thread->state == TS_RUNNING)
        thread_enqueue(running_addr());
    thread_switch(to, running_addr());
}

void thread_block()
{
    thread *to = thread_sched();
    thread_switch(to, running_addr());
}

void thread_block_irqs_disabled() { thread_block(); }

noreturn void thread_done()
{
    thread *to = thread_sched();
    thread_switch(to, running_addr());
    UNREACHABLE();
}

noreturn void thread_done_irqs_disabled() { thread_done(); }

static bool needs_fpu(thread *th) { return th->proc->pid != 0; }

static bool change_vm(thread *new_thread, thread *old_thread)
{
    return new_thread->proc->vm_root != old_thread->proc->vm_root
        && !(new_thread->flags & TF_IS_KTHREAD);
}

enum in_out { SCH_IN, SCH_OUT };

static void account_thread(thread *th, enum in_out st)
{
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

void thread_switch(thread *restrict new_thread, thread *restrict old_thread)
{
    set_kernel_stack(new_thread->kstack);

    if (needs_fpu(old_thread))
        fxsave(&old_thread->fpctx);
    if (needs_fpu(new_thread))
        fxrstor(&new_thread->fpctx);
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
        old_thread->remove_flag(TF_ON_CPU);
        if (new_thread->tlsbase)
            set_tls_base(new_thread->tlsbase);
        if (!(old_thread->flags & TF_IS_KTHREAD))
            old_thread->irq_disable_depth += 1;
        if (!(running_thread->flags & TF_IS_KTHREAD)) {
            handle_killed_condition();
            handle_pending_signals();
            handle_stopped_condition();
        }
        if (running_thread->state != TS_RUNNING)
            thread_block();
        if (!(running_thread->flags & TF_IS_KTHREAD))
            enable_irqs();
        return;
    }
    account_thread(old_thread, SCH_OUT);
    account_thread(new_thread, SCH_IN);
    longjmp(new_thread->kernel_ctx, 1);
}

noreturn void thread_switch_no_save(thread *new_thread)
{
    set_kernel_stack(new_thread->kstack);

    if (needs_fpu(new_thread))
        fxrstor(&new_thread->fpctx);
    set_vm_root(new_thread->proc->vm_root);
    thread_set_running(new_thread);
    longjmp(new_thread->kernel_ctx, 1);
}

static void *new_kernel_stack()
{
    char *new_stack = static_cast<char *>(vmm_reserve(THREAD_STACK_SIZE));
    // touch the pages so they exist before we swap to this stack
    memset(new_stack, 0, THREAD_STACK_SIZE);
    void *stack_top = new_stack + THREAD_STACK_SIZE;
    return stack_top;
}

static void free_kernel_stack(thread *th)
{
    vmm_unmap_range(
        ((uintptr_t)th->kstack) - THREAD_STACK_SIZE, THREAD_STACK_SIZE);
}

static noreturn void thread_entrypoint()
{
    thread *th = running_addr();

    th->entry(th->entry_arg);
    UNREACHABLE();
}

static pid_t assign_tid(thread *th)
{
    for (pid_t i = 0; i < threads.size(); i++) {
        if (threads[i] == nullptr) {
            threads[i] = th;
            return i;
        }
    }
    threads.push_back(th);
    return threads.size() - 1;
}

static thread *new_thread()
{
    thread *th = new_thread_slot();
    pid_t new_tid = assign_tid(th);
    DEBUG_PRINTF("new_thread = %i\n", new_tid);

    memset(th, 0, sizeof(thread));
    th->state = TS_PREINIT;

    all_threads.push_back(*th);

    th->kstack = (char *)new_kernel_stack();
    th->kernel_ctx->__regs.sp = (uintptr_t)th->kstack - 8;
    th->kernel_ctx->__regs.bp = (uintptr_t)th->kstack - 8;
    th->kernel_ctx->__regs.ip = (uintptr_t)thread_entrypoint;

    th->tid = new_tid;
    th->irq_disable_depth = 1;
    th->magic = thread_magic;
    th->tlsbase = nullptr;
    th->report_events = running_thread->report_events;
    // th->flags = TF_SYSCALL_TRACE;
    // th->add_flag(TF_SYSCALL_TRACE_CHILDREN);

    make_proc_directory(th);

    log_event(EVENT_THREAD_NEW, "new thread: %i\n", new_tid);

    return th;
}

thread *kthread_create(void (*entry)(void *), void *arg)
{
    DEBUG_PRINTF("new_kernel_thread(%p)\n", entry);

    thread *th = new_thread();

    th->entry = entry;
    th->entry_arg = arg;
    th->proc = process_by_id(0);
    th->flags = TF_IS_KTHREAD, th->proc->threads.push_back(*th);

    th->state = TS_STARTED;
    thread_enqueue(th);
    return th;
}

thread *process_thread(process *p) { return &p->threads.front(); }

static process *new_process(thread *th)
{
    process *proc = new_process_slot();
    memset(proc, 0, sizeof(process));
    proc->magic = proc_magic;

    proc->root = global_root_dentry;

    proc->pid = th->tid;
    proc->parent = running_process;
    th->proc = proc;

    running_process->children.push_back(*proc);
    proc->threads.push_back(*th);

    return proc;
}

static void new_userspace_entry(void *filename_v)
{
    const char *filename = static_cast<const char *>(filename_v);
    interrupt_frame *frame
        = (interrupt_frame *)(USER_STACK - 16 - sizeof(interrupt_frame));
    sysret err = sys_execve(frame, filename, nullptr, nullptr);
    assert(err == 0 && "BOOTSTRAP ERROR");

    asm volatile("mov %0, %%rsp \n\t"
                 "jmp return_from_interrupt \n\t"
                 :
                 : "rm"(frame));

    // jmp_to_userspace(frame->ip, frame->user_sp, 0, 0);
    UNREACHABLE();
}

void bootstrap_usermode(const char *init_filename)
{
    threads[1] = nullptr;
    thread *th = new_thread();
    process *proc = new_process(th);

    th->entry = new_userspace_entry;
    th->entry_arg = (void *)init_filename;
    th->cwd = resolve_path("/bin");

    proc->mmap_base = USER_MMAP_BASE;
    proc->vm_root = vmm_fork(proc);

    th->state = TS_RUNNING;

    thread_enqueue(th);
}

sysret sys_create(const char *executable)
{
    return -ETODO; // not working with fs2
    thread *th = new_thread();
    process *proc = new_process(th);

    th->entry = new_userspace_entry;
    th->entry_arg = (void *)executable;
    th->cwd = resolve_path("/bin");

    proc->mmap_base = USER_MMAP_BASE;
    proc->vm_root = vmm_fork(proc);
    proc->parent = process_by_id(1);

    return proc->pid;
}

sysret sys_procstate(pid_t destination, enum procstate flags)
{
    process *d_p = process_by_id(destination);
    if (!d_p)
        return -ESRCH;
    if (d_p == ZOMBIE)
        return -ESRCH;
    process *p = running_process;

    if (flags & PS_COPYFDS) {
        // clone_all_files_to(d_p)
    }

    if (flags & PS_SETRUN) {
        thread *th = &d_p->threads.front();
        th->state = TS_RUNNING;
        thread_enqueue(th);
    }

    return 0;
}

noreturn static void finalizer_kthread(void *_)
{
    while (true) {
        thread *th;

        if (freeable_thread_queue.empty()) {
            thread_block();
        } else {
            th = &freeable_thread_queue.front();
            freeable_thread_queue.remove(*th);
            free_kernel_stack(th);
            free_thread_slot(th);
        }
    }
}

static int process_matches(pid_t wait_arg, process *proc)
{
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

static void wake_waiting_parent_thread()
{
    if (running_process->pid == 0)
        return;
    process *parent = running_process->parent;
    for (auto &parent_th : parent->threads) {
        if (parent_th.state == TS_WAIT
            && process_matches(parent_th.wait_request, running_process)) {
            parent_th.wait_result = running_process;
            parent_th.state = TS_RUNNING;
            signal_send_th(&parent_th, SIGCHLD);
            return;
        }
    }

    // no one is listening, signal the tg leader
    thread *parent_th = process_thread(parent);
    signal_send_th(parent_th, SIGCHLD);
}

static void do_process_exit(int exit_status)
{
    if (running_process->pid == 1)
        panic("attempted to kill init!");
    assert(running_process->threads.empty());
    running_process->exit_status = exit_status + 1;

    process *init = process_by_id(1);
    if (!running_process->children.empty()) {
        for (auto &child : running_process->children) {
            child.parent = init;
            init->children.push_back(child);
        }
    }

    wake_waiting_parent_thread();
}

static noreturn void do_thread_exit(int exit_status)
{
    DEBUG_PRINTF("do_thread_exit(%i)\n", exit_status);
    assert(running_thread->state != TS_DEAD);

    if (running_thread->tracer)
        running_thread->tracer->tracees.remove(*running_addr());
    running_process->threads.remove(*running_addr());
    all_threads.remove(*running_addr());
    runnable_thread_queue.remove(*running_addr());

    if (running_thread->wait_event) {
        drop_timer_event(running_addr()->wait_event);
    }

    if (running_thread->tid == running_process->pid) {
        running_process->exit_intention = exit_status + 1;
        threads[running_thread->tid] = (thread *)ZOMBIE;
    } else {
        threads[running_thread->tid] = nullptr;
    }

    log_event(EVENT_THREAD_DIE, "die thread: %i\n", running_thread->tid);

    if (running_process->threads.empty())
        do_process_exit(exit_status);

    destroy_proc_directory(running_thread->proc_dir);

    running_thread->state = TS_DEAD;
    make_freeable(running_addr());
    thread_done_irqs_disabled();
}

noreturn sysret sys__exit(int exit_status)
{
    kill_process(running_process, exit_status);
    UNREACHABLE();
}

noreturn sysret sys_exit_thread(int exit_status)
{
    do_thread_exit(exit_status);
}

noreturn void kthread_exit() { do_thread_exit(0); }

extern nx::vector<file *> clone_all_files(process *p);

extern "C" sysret sys_fork(interrupt_frame *r)
{
    DEBUG_PRINTF("sys_fork(%p)\n", r);

    if (running_process->pid == 0)
        panic("Cannot fork() the kernel\n");

    thread *new_th = new_thread();
    process *new_proc = new_process(new_th);

    strncpy(new_proc->comm, running_process->comm, COMM_SIZE);
    new_proc->pgid = running_process->pgid;
    new_proc->uid = running_process->uid;
    new_proc->gid = running_process->gid;
    new_proc->mmap_base = running_process->mmap_base;
    new_proc->elf_metadata = clone_elf_md(running_process->elf_metadata);

    // copy files to child
    new_proc->files = clone_all_files(running_process);

    new_th->user_sp = running_thread->user_sp;

    new_th->proc = new_proc;
    new_th->flags = running_thread->flags;
    new_th->cwd = running_thread->cwd;
    if (!(running_thread->flags & TF_SYSCALL_TRACE_CHILDREN)) {
        new_th->remove_flag(TF_SYSCALL_TRACE);
    }

    interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
    memcpy(frame, r, sizeof(interrupt_frame));
    FRAME_RETURN(frame) = 0;
    new_th->user_ctx = frame;
    new_th->add_flag(TF_USER_CTX_VALID);

    new_th->kernel_ctx->__regs.ip = (uintptr_t)return_from_interrupt;
    new_th->kernel_ctx->__regs.sp = (uintptr_t)new_th->user_ctx;
    new_th->kernel_ctx->__regs.bp = (uintptr_t)new_th->user_ctx;

    new_proc->vm_root = vmm_fork(new_proc);
    new_th->state = TS_STARTED;
    new_th->irq_disable_depth = running_thread->irq_disable_depth;

    thread_enqueue(new_th);
    return new_proc->pid;
}

sysret sys_clone0(interrupt_frame *r, int (*fn)(void *), void *new_stack,
    int flags, void *arg)
{
    DEBUG_PRINTF(
        "sys_clone0(%p, %p, %p, %p, %i)\n", r, fn, new_stack, arg, flags);

    if (running_process->pid == 0) {
        panic("Cannot clone() the kernel - you want kthread_create\n");
    }

    thread *new_th = new_thread();

    running_process->threads.push_back(*new_th);

    new_th->proc = running_process;
    new_th->flags = running_thread->flags;
    // new_th->cwd = running_thread->cwd;
    new_th->cwd = running_thread->cwd;

    interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
    memcpy(frame, r, sizeof(interrupt_frame));
    FRAME_RETURN(frame) = 0;
    new_th->user_ctx = frame;
    new_th->add_flag(TF_USER_CTX_VALID);

    frame->user_sp = (uintptr_t)new_stack;
    frame->bp = (uintptr_t)new_stack;
    frame->ip = (uintptr_t)fn;

    new_th->kernel_ctx->__regs.ip = (uintptr_t)return_from_interrupt;
    new_th->kernel_ctx->__regs.sp = (uintptr_t)new_th->user_ctx;
    new_th->kernel_ctx->__regs.bp = (uintptr_t)new_th->user_ctx;

    new_th->state = TS_STARTED;

    thread_enqueue(new_th);

    return new_th->tid;
}

sysret sys_getpid() { return running_process->pid; }

sysret sys_gettid() { return running_thread->tid; }

static void destroy_child_process(process *proc)
{
    assert(proc != running_process);
    assert(proc->exit_status);
    void *child_thread = threads[proc->pid];
    assert(child_thread == ZOMBIE);
    threads[proc->pid] = nullptr;

    // ONE OF THESE IS WRONG
    assert(proc->threads.empty());
    if (proc->parent)
        proc->parent->children.remove(*proc);

    close_all_files(proc);

    vmm_destroy_tree(proc->vm_root);
    if (proc->elf_metadata)
        free(proc->elf_metadata);
    proc->elf_metadata = nullptr;
    free_process_slot(proc);
}

// If it finds a child process to destroy, find_dead_child returns with
// interrupts disabled. destroy_child_process will re-enable them.
static process *find_dead_child(pid_t query)
{
    for (auto &child : running_process->children) {
        if (!process_matches(query, &child))
            continue;
        if (child.exit_status > 0)
            return &child;
    }
    return nullptr;
}

static thread *find_waiting_tracee(pid_t query)
{
    for (auto &th : running_thread->tracees) {
        if (!process_matches(query, th.proc))
            continue;
        if (th.state == TS_TRWAIT)
            return &th;
    }
    return nullptr;
}

static void wait_for(pid_t pid)
{
    running_thread->state = TS_WAIT;
    running_thread->wait_request = pid;
    running_thread->wait_result = nullptr;
    running_thread->wait_trace_result = nullptr;
}

static void clear_wait()
{
    running_thread->wait_request = 0;
    running_thread->wait_result = nullptr;
    running_thread->wait_trace_result = nullptr;
    running_thread->state = TS_RUNNING;
}

sysret sys_waitpid(pid_t pid, int *status, enum wait_options options)
{
    DEBUG_PRINTF("[%i] waitpid(%i, xx, xx)\n", running_thread->tid, pid);

    int exit_code;
    int found_pid;

    wait_for(pid);

    process *child = find_dead_child(pid);
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

    thread *trace_th = find_waiting_tracee(pid);
    if (trace_th) {
        clear_wait();

        if (status)
            *status = trace_th->trace_report;
        return trace_th->tid;
    }

    if (running_process->children.empty() && running_thread->tracees.empty()) {
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

    process *p = running_thread->wait_result;
    thread *t = running_thread->wait_trace_result;
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

sysret sys_syscall_trace(pid_t tid, int state)
{
    thread *th;
    if (tid == 0) {
        th = running_addr();
    } else {
        th = thread_by_id(tid);
    }
    if (!th)
        return -ESRCH;

    if (state == 0) {
        th->remove_flag(TF_SYSCALL_TRACE);
        th->remove_flag(TF_SYSCALL_TRACE_CHILDREN);
    }
    if (state & 1)
        th->add_flag(TF_SYSCALL_TRACE);
    if (state & 2)
        th->add_flag(TF_SYSCALL_TRACE_CHILDREN);

    return state;
}

sysret sys_yield()
{
    thread_yield();
    return 0;
}

sysret sys_setpgid(int pid, int pgid)
{
    process *proc;
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

sysret sys_exit_group(int exit_status)
{
    // kill_process_group(running_process->pgid); // TODO
    kill_process(running_process, exit_status);
    UNREACHABLE();
}

static void handle_killed_condition()
{
    if (running_thread->state == TS_DEAD)
        return;
    if (running_process->exit_intention) {
        do_thread_exit(running_process->exit_intention - 1);
    }
}

void kill_process(process *p, int reason)
{
    thread *th, *tmp;

    if (p->threads.empty())
        return;
    p->exit_intention = reason + 1;

    if (p == running_process)
        do_thread_exit(reason);
}

void kill_pid(pid_t pid)
{
    process *p = process_by_id(pid);
    if (!p)
        return;
    if (p == ZOMBIE)
        return;
    kill_process(p, 0);
}

static void handle_stopped_condition()
{
    while (running_thread->flags & TF_STOPPED)
        thread_block();
}

__USED
static void print_thread(thread *th)
{
    const char *status;
    switch (th->state) {
    default:
        status = "?";
        break;
    }

    printf("  t: %i %s%s%s\n", th->tid, "", status, " TODO");
}

__USED
static void print_process(void *p)
{
    process *proc = static_cast<process *>(p);

    if (proc->exit_status <= 0) {
        printf("pid %i: %s\n", proc->pid, proc->comm);
    } else {
        printf("pid %i: %s (defunct: %i)\n", proc->pid, proc->comm,
            proc->exit_status);
    }

    for (auto &th : proc->threads) {
        print_thread(&th);
    }
}

sysret sys_top(int show_threads)
{
    for (auto &th : all_threads) {
        printf("  %i:%i '%s'\n", th.proc->pid, th.tid, th.proc->comm);
    }
    return 0;
}

void unsleep_thread(thread *t)
{
    t->wait_event = nullptr;
    t->state = TS_RUNNING;
    thread_enqueue(t);
}

static void unsleep_thread_callback(void *t)
{
    unsleep_thread(static_cast<thread *>(t));
}

void sleep_thread(int ms)
{
    assert(running_thread->tid != 0);
    int ticks = milliseconds(ms);
    timer_event *te
        = insert_timer_event(ticks, unsleep_thread_callback, running_addr());
    running_thread->wait_event = te;
    running_thread->state = TS_SLEEP;
    thread_block();
}

sysret sys_sleepms(int ms)
{
    sleep_thread(ms);
    return 0;
}

void thread_timer(void *_)
{
    insert_timer_event(THREAD_TIME, thread_timer, nullptr);
    thread_yield();
}

bool user_map(virt_addr_t base, virt_addr_t top)
{
    mm_region *slot = nullptr, *test;
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
    slot->flags = {};
    slot->inode = nullptr;

    vmm_create_unbacked_range(base, top - base, PAGE_WRITEABLE | PAGE_USERMODE);
    return true;
}

void proc_threads(file *ofd, void *_)
{
    proc_sprintf(ofd, "tid pid ppid comm\n");
    for (auto &th : all_threads) {
        process *p = th.proc;
        process *pp = p->parent;
        pid_t ppid = pp ? pp->pid : -1;
        proc_sprintf(ofd, "%i %i %i %s\n", th.tid, p->pid, ppid, p->comm);
    }
}

void proc_threads_detail(file *ofd, void *_)
{
    proc_sprintf(ofd, "%15s %5s %5s %5s %7s %7s %15s %7s\n", "comm", "tid",
        "pid", "ppid", "n_sched", "time", "tsc", "tsc/1B");
    for (auto &th : all_threads) {
        process *p = th.proc;
        process *pp = p->parent;
        pid_t ppid = pp ? pp->pid : -1;
        proc_sprintf(ofd, "%15s %5i %5i %5i %7li %7li %15li %7li\n", p->comm,
            th.tid, p->pid, ppid, th.n_scheduled, th.time_ran, th.tsc_ran,
            th.tsc_ran / 1000000000L);
    }
}

void proc_zombies(file *ofd, void *_)
{
    for (auto &th : threads) {
        if ((void *)th == ZOMBIE)
            proc_sprintf(ofd, "%li ", &th - threads.begin());
    }
    proc_sprintf(ofd, "\n");
}

void print_cpu_info()
{
    printf(
        "running thread [%i:%i]\n", running_thread->tid, running_process->pid);
}

void proc_comm(file *file, void *arg);
void proc_fds(file *file, void *arg);
void proc_stack(file *file, void *arg);
ssize_t proc_fds_getdents(file *file, dirent *buf, size_t len);
dentry *proc_fds_lookup(dentry *dent, const char *child);

file_operations proc_fds_ops = {
    .getdents = proc_fds_getdents,
};

inode_operations proc_fds_inode_ops = {
    .lookup = proc_fds_lookup,
};

struct proc_spec {
    const char *name;
    void (*func)(file *, void *);
    mode_t mode;
} proc_spec[] = {
    { "comm", proc_comm, 0444 },
    { "fds", proc_fds, 0444 },
    { "stack", proc_stack, 0444 },
};

void make_proc_directory(thread *thread)
{
    inode *dir = new_inode(proc_file_system, _NG_DIR | 0555);
    dentry *ddir = proc_file_system->root;
    char name[32];
    sprintf(name, "%i", thread->tid);
    ddir = add_child(ddir, name, dir);
    if (IS_ERROR(ddir))
        return;

    for (auto &spec : proc_spec) {
        inode *inode = new_proc_inode(spec.mode, spec.func, thread);
        add_child(ddir, spec.name, inode);
    }

    inode *inode = new_inode(proc_file_system, _NG_DIR | 0555);
    extern file_operations proc_dir_file_ops;
    inode->file_ops = &proc_fds_ops;
    inode->ops = &proc_fds_inode_ops;
    inode->data = thread;
    add_child(ddir, "fds2", inode);

    thread->proc_dir = ddir;
}

void destroy_proc_directory(dentry *proc_dir)
{
    unlink_dentry(proc_dir);
    list_for_each (dentry, d, &proc_dir->children, children_node) {
        unlink_dentry(d);
    }
    maybe_delete_dentry(proc_dir);
}

void proc_comm(file *file, void *arg)
{
    auto *th = static_cast<thread *>(arg);

    proc_sprintf(file, "%s\n", th->proc->comm);
}

void proc_fds(file *file, void *arg)
{
    auto *th = static_cast<thread *>(arg);
    char buffer[128] = { 0 };

    for (auto &f : th->proc->files) {
        if (!f)
            continue;
        dentry *d = f->dentry;
        inode *n = f->inode;
        int i = &f - th->proc->files.begin();
        if (!d) {
            proc_sprintf(file, "%i %c@%i\n", i, __filetype_sigils[n->type],
                n->inode_number);
        } else {
            pathname(d, buffer, 128);
            proc_sprintf(file, "%i %s\n", i, buffer);
        }
    }
}

mode_t proc_fd_mode(file *file)
{
    int mode = 0;
    if (file->flags & O_RDONLY)
        mode |= USR_READ;
    if (file->flags & O_WRONLY)
        mode |= USR_WRITE;
    return mode;
}

ssize_t proc_fds_getdents(file *file, dirent *buf, size_t len)
{
    auto *th = static_cast<thread *>(file->inode->data);
    size_t offset = 0;
    for (auto &f : th->proc->files) {
        auto *d = static_cast<dirent *>(PTR_ADD(buf, offset));
        if (!f)
            continue;
        int i = &f - th->proc->files.begin();
        int namelen = snprintf(d->d_name, 64, "%i", i);
        d->d_type = FT_SYMLINK;
        d->d_mode = proc_fd_mode(f);
        d->d_ino = 0;
        d->d_off = 0;
        size_t reclen = sizeof(dirent) - 256 + ROUND_UP(namelen + 1, 8);
        d->d_reclen = reclen;
        offset += reclen;
    }

    return offset;
}

ssize_t proc_fd_readlink(inode *inode, char *buffer, size_t len);

inode_operations proc_fd_ops = {
    .readlink = proc_fd_readlink,
};

dentry *proc_fds_lookup(dentry *dent, const char *child)
{
    thread *th = static_cast<thread *>(dent->inode->data);
    char *end;
    int fd = strtol(child, &end, 10);
    if (*end)
        return static_cast<dentry *>(TO_ERROR(-ENOENT));
    if (fd > th->proc->files.size())
        return static_cast<dentry *>(TO_ERROR(-ENOENT));
    if (!th->proc->files[fd])
        return static_cast<dentry *>(TO_ERROR(-ENOENT));
    inode *inode = new_inode(
        proc_file_system, _NG_SYMLINK | proc_fd_mode(th->proc->files[fd]));
    inode->ops = &proc_fd_ops;
    inode->extra = th->proc->files[fd];
    dentry *ndentry = new_dentry();
    attach_inode(ndentry, inode);
    ndentry->name = strdup(child);
    return ndentry;
}

ssize_t proc_fd_readlink(inode *inode, char *buffer, size_t len)
{
    auto *f = static_cast<file *>(inode->extra);
    if (f->dentry) {
        memset(buffer, 0, len);
        return pathname(f->dentry, buffer, len);
    } else {
        const char *type = __filetype_names[f->inode->type];
        return snprintf(buffer, len, "%s:[%i]", type, f->inode->inode_number);
    }
}

ssize_t proc_self_readlink(inode *inode, char *buffer, size_t len)
{
    return snprintf(buffer, len, "%i", running_thread->tid);
}

inode_operations proc_self_ops = {
    .readlink = proc_self_readlink,
};

#include <elf.h>
#include <ng/mod.h>

void proc_backtrace_callback(uintptr_t bp, uintptr_t ip, void *arg)
{
    auto *f = static_cast<file *>(arg);
    mod_sym sym = elf_find_symbol_by_address(ip);
    if (ip > HIGHER_HALF && sym.sym) {
        const elf_md *md = sym.mod ? sym.mod->md : &elf_ngk_md;
        const char *name = elf_symbol_name(md, sym.sym);
        ptrdiff_t offset = ip - sym.sym->st_value;
        if (sym.mod) {
            proc_sprintf(f, "(%#018zx) <%s:%s+%#tx> (%s @ %#018tx)\n", ip,
                sym.mod->name, name, offset, sym.mod->name, sym.mod->load_base);
        } else {
            proc_sprintf(f, "(%#018zx) <%s+%#tx>\n", ip, name, offset);
        }
    } else if (ip != 0) {
        const elf_md *md = running_process->elf_metadata;
        if (!md) {
            proc_sprintf(f, "(%#018zx) <?+?>\n", ip);
            return;
        }
        const Elf_Sym *sym = elf_symbol_by_address(md, ip);
        if (!sym) {
            proc_sprintf(f, "(%#018zx) <?+?>\n", ip);
            return;
        }
        const char *name = elf_symbol_name(md, sym);
        ptrdiff_t offset = ip - sym->st_value;
        proc_sprintf(f, "(%#018zx) <%s+%#tx>\n", ip, name, offset);
    }
}

void proc_backtrace_from_with_ip(file *file, thread *thread)
{
    backtrace(thread->kernel_ctx->__regs.bp, thread->kernel_ctx->__regs.ip,
        proc_backtrace_callback, file);
}

void proc_stack(file *file, void *arg)
{
    auto *th = static_cast<thread *>(arg);
    proc_backtrace_from_with_ip(file, th);
}

sysret sys_settls(void *tlsbase)
{
    running_thread->tlsbase = tlsbase;
    set_tls_base(tlsbase);
    return 0;
}

sysret sys_report_events(long event_mask)
{
    running_thread->report_events = event_mask;
    return 0;
}

void proc_cpus(file *file, void *arg)
{
    (void)arg;
    proc_sprintf(file, "%10s %10s\n", "cpu", "running");

    for (int i = 0; i < NCPUS; i++) {
        if (!thread_cpus[i])
            continue;
        proc_sprintf(file, "%10i %10i\n", i, thread_cpus[i]->running->tid);
    }
}

// temporary for C compatibility

pid_t get_running_pid() { return running_process->pid; }

pid_t get_running_tid() { return running_thread->tid; }

phys_addr_t get_running_pt_root() { return running_process->vm_root; }

void set_running_pt_root(phys_addr_t new_root)
{
    running_process->vm_root = new_root;
}

struct dentry *get_running_cwd() { return running_thread->cwd; }

void set_running_cwd(struct dentry *new_cwd) { running_thread->cwd = new_cwd; }

struct dentry *get_running_root() { return running_process->root; }

uint64_t get_running_report_events() { return running_thread->report_events; }

virt_addr_t allocate_mmap_space(size_t size)
{
    virt_addr_t base = running_process->mmap_base;
    running_process->mmap_base += size;
    return base;
}

void copy_running_mem_regions_to(struct process *to)
{
    memcpy(to->mm_regions, running_process->mm_regions,
        sizeof(running_process->mm_regions));
}

elf_md *get_running_elf_metadata() { return running_process->elf_metadata; }