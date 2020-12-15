// #define DEBUG
#include <basic.h>
#include <elf.h>
#include <errno.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/mutex.h>
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
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define THREAD_STACK_SIZE 0x2000
extern uintptr_t boot_pt_root;

LIST_DEFINE(all_threads);
LIST_DEFINE(runnable_thread_queue);
struct mutex runnable_lock = MTX_INIT(runnable_lock);
LIST_DEFINE(freeable_thread_queue);
struct thread *finalizer = NULL;

extern struct tar_header *initfs;

#define THREAD_TIME milliseconds(5)

// kmutex process_lock = KMUTEX_INIT;
struct dmgr threads;

static void finalizer_kthread(void *);
static void thread_timer(void *);
static void handle_killed_condition();
static void handle_stopped_condition();
static noreturn void do_process_exit(int);
void proc_threads(struct open_file *ofd, void *_);

struct process proc_zero = {
    .pid = 0,
    .magic = PROC_MAGIC,
    .comm = "<nightingale>",
    .vm_root = (uintptr_t)&boot_pt_root,
    .parent = NULL,
    .children = LIST_INIT(proc_zero.children),
    .threads = LIST_INIT(proc_zero.threads),
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
    .tid = 0,
    .magic = THREAD_MAGIC,
    .kstack = &boot_kernel_stack,
    .state = TS_RUNNING,
    .flags = TF_IS_KTHREAD | TF_ONCPU,
    .irq_disable_depth = 1,
};

struct thread *thread_idle = &thread_zero;

struct process *running_process = &proc_zero;
struct thread *running_thread = &thread_zero;

static struct process *new_process_slot() {
    return malloc(sizeof(struct process));
}

static struct thread *new_thread_slot() {
    return malloc(sizeof(struct thread));
}

static void free_process_slot(struct process *defunct) {
    free(defunct);
}

static void free_thread_slot(struct thread *defunct) {
    assert(defunct->state == TS_DEAD);
    free(defunct);
}

struct thread *thread_by_id(pid_t tid) {
    struct thread *th = dmgr_get(&threads, tid);
    return th;
}

struct process *process_by_id(pid_t pid) {
    struct thread *th = thread_by_id(pid);
    return th ? th->proc : NULL;
}

void threads_init() {
    DEBUG_PRINTF("init_threads()\n");

    dmgr_init(&threads);

    thread_zero.proc = &proc_zero;

    dmgr_insert(&threads, &thread_zero);
    dmgr_insert(&threads, (void *)1); // save 1 for init

    list_append(&all_threads, &thread_zero.all_threads);
    list_append(&proc_zero.threads, &thread_zero.process_threads);

    make_procfile("threads", proc_threads, NULL);

    printf("threads: process structures initialized\n");

    finalizer = kthread_create(finalizer_kthread, NULL);
    printf("threads: finalizer thread running\n");

    insert_timer_event(milliseconds(10), thread_timer, NULL);
    printf("threads: thread_timer started\n");
}

static struct interrupt_frame *thread_frame(struct thread *th) {
    if (th->flags & TF_USER_CTX_VALID) {
        return th->user_ctx;
    } else {
        return NULL;
    }
}

void set_kernel_stack(void *stack_top) {
    extern uintptr_t *kernel_stack;
    *&kernel_stack = stack_top;
}

static void make_freeable(struct thread *defunct) {
    assert(defunct->state == TS_DEAD);
    assert(defunct->freeable.next == NULL);
    DEBUG_PRINTF("freeable(%i)\n", defunct->tid);
    list_append(&freeable_thread_queue, &defunct->freeable);
    thread_enqueue(finalizer);
}

static bool enqueue_checks(struct thread *th) {
    if (th->tid == 0) return false;
    // if (th->trace_state == TRACE_STOPPED)  return false;
    // I hope the above is covered by TRWAIT, but we'll see
    if (th->flags & TF_QUEUED) return false;
    assert(th->proc->pid > -1);
    assert(th->magic == THREAD_MAGIC);
    th->flags |= TF_QUEUED;
    return true;
}

void thread_enqueue(struct thread *th) {
    disable_irqs();
    if (enqueue_checks(th)) {
        with_lock(&runnable_lock) {
            _list_append(&runnable_thread_queue, &th->runnable);
        }
    }
    enable_irqs();
}

void thread_enqueue_at_front(struct thread *th) {
    disable_irqs();
    if (enqueue_checks(th)) {
        with_lock(&runnable_lock) {
            _list_prepend(&runnable_thread_queue, &th->runnable);
        }
    }
    enable_irqs();
}

// portability!
static void fxsave(fp_ctx *fpctx) {
    // printf("called fxsave with %p\n", fpctx);
#if X86_64
    asm volatile("fxsaveq %0" ::"m"(*fpctx));
#endif
}

static void fxrstor(fp_ctx *fpctx) {
    // printf("called fxrstor with %p\n", fpctx);
#if X86_64
    asm volatile("fxrstorq %0" : "=m"(*fpctx));
#endif
}

static struct thread *next_runnable_thread() {
    if (list_empty(&runnable_thread_queue)) return NULL;
    struct thread *rt;
    with_lock(&runnable_lock) {
        rt = list_pop_front(struct thread, runnable, &runnable_thread_queue);
    }
    rt->flags &= ~TF_QUEUED;
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
struct thread *thread_sched(void) {
    disable_irqs();

    struct thread *to;
    to = next_runnable_thread();

    if (!to) to = thread_idle;
    assert(to->magic == THREAD_MAGIC);
    return to;
}

static void thread_set_running(struct thread *th) {
    running_process = th->proc;
    running_thread = th;
    th->flags |= TF_ONCPU;
    if (th->state == TS_STARTED) th->state = TS_RUNNING;
}


void thread_yield(void) {
    struct thread *to = thread_sched();
    if (to == thread_idle) {
        enable_irqs();
        return;
    }

    thread_enqueue(running_thread);
    thread_switch(to, running_thread);
}

void thread_block(void) {
    struct thread *to = thread_sched();
    thread_switch(to, running_thread);
}

noreturn void thread_done(void) {
    struct thread *to = thread_sched();
    thread_switch(to, running_thread);
    UNREACHABLE();
}


static bool needs_fpu(struct thread *th) {
    return th->proc->pid != 0;
}

static bool change_vm(struct thread *new, struct thread *old) {
    return new->proc->vm_root != old->proc->vm_root &&
           !(new->flags &TF_IS_KTHREAD);
}

enum in_out { SCH_IN, SCH_OUT };

static void account_thread(struct thread *th, enum in_out st) {
    uint64_t tick_time = kernel_timer;
    long long tsc_time = rdtsc();

    if (st == SCH_IN) {
        th->n_scheduled += 1;
        th->last_scheduled = tick_time;
        th->tsc_scheduled = tsc_time;
    } else {
        th->time_ran += tick_time - th->last_scheduled;
        th->tsc_ran += tsc_time - th->tsc_scheduled;
    }
}

void thread_switch(struct thread *restrict new, struct thread *restrict old) {
    set_kernel_stack(new->kstack);

    // Thought: can I store the FPU state in the stack frame of this
    // function? That may help signal frame handling, and would slim
    // down thread a ton!

    if (needs_fpu(old)) { fxsave(&old->fpctx); }
    if (needs_fpu(new)) { fxrstor(&new->fpctx); }

    if (change_vm(new, old)) { set_vm_root(new->proc->vm_root); }

    DEBUG_PRINTF("[%i:%i] -> [%i:%i]\n", old->proc->pid, old->tid,
                 new->proc->pid, new->tid);

    thread_set_running(new);

    if (setjmp(old->kernel_ctx)) {
        account_thread(new, SCH_IN);
        old->flags &= ~TF_ONCPU;
        enable_irqs();
        handle_killed_condition();
        handle_pending_signals();
        handle_stopped_condition();
        if (running_thread->state != TS_RUNNING) { thread_block(); }
        return;
    }
    account_thread(old, SCH_OUT);
    longjmp(new->kernel_ctx, 1);
}

noreturn void thread_switch_nosave(struct thread *new) {
    set_kernel_stack(new->kstack);

    if (needs_fpu(new)) { fxrstor(&new->fpctx); }
    set_vm_root(new->proc->vm_root);
    thread_set_running(new);
    longjmp(new->kernel_ctx, 1);
}

static void thread_procfile(struct open_file *ofd) {}

static void destroy_thread_procfile(struct thread *th) {}

static void *new_kernel_stack() {
    char *new_stack = vmm_reserve(THREAD_STACK_SIZE);
    // touch the pages so they exist before we swap to this stack
    memset(new_stack, 0, THREAD_STACK_SIZE);
    void *stack_top = new_stack + THREAD_STACK_SIZE;
    return stack_top;
}

static void free_kernel_stack(struct thread *th) {
    vmm_unmap_range(((uintptr_t)th->kstack) - THREAD_STACK_SIZE,
                    THREAD_STACK_SIZE);
}

static noreturn void thread_entrypoint(void) {
    struct thread *th = running_thread;

    enable_irqs();
    th->entry(th->entry_arg);
    UNREACHABLE();
}

static struct thread *new_thread() {
    struct thread *th = new_thread_slot();
    int new_tid = dmgr_insert(&threads, th);
    memset(th, 0, sizeof(struct thread));
    th->state = TS_PREINIT;

    list_init(&th->tracees);
    list_init(&th->process_threads);
    list_append(&all_threads, &th->all_threads);

    th->kstack = (char *)new_kernel_stack();
    th->kernel_ctx->__regs.sp = (uintptr_t)th->kstack - 16;
    th->kernel_ctx->__regs.bp = (uintptr_t)th->kstack - 16;
    th->kernel_ctx->__regs.ip = (uintptr_t)thread_entrypoint;

    th->tid = new_tid;
    th->irq_disable_depth = 1;
    // th->procfile = make_thread_procfile(th);
    th->magic = THREAD_MAGIC;
    // th->flags = TF_SYSCALL_TRACE;

    return th;
}

struct thread *kthread_create(void (*entry)(void *), void *arg) {
    DEBUG_PRINTF("new_kernel_thread(%p)\n", entry);

    struct thread *th = new_thread();

    th->entry = entry;
    th->entry_arg = arg;
    th->proc = &proc_zero;
    th->flags = TF_IS_KTHREAD,
    list_append(&proc_zero.threads, &th->process_threads);

    th->state = TS_STARTED;
    thread_enqueue(th);
    return th;
}

struct thread *process_thread(struct process *p) {
    return list_head(struct thread, process_threads, &p->threads);
}

static struct process *new_process(struct thread *th) {
    struct process *proc = new_process_slot();
    memset(proc, 0, sizeof(struct process));
    proc->magic = PROC_MAGIC;

    list_init(&proc->children);
    list_init(&proc->threads);
    dmgr_init(&proc->fds);

    proc->pid = th->tid;
    proc->parent = running_process;
    th->proc = proc;

    list_append(&running_process->children, &proc->siblings);
    list_append(&proc->threads, &th->process_threads);

    return proc;
}

static void new_userspace_entry(void *filename) {
    interrupt_frame *frame =
        (void *)(USER_STACK - 16 - sizeof(interrupt_frame));
    sysret err = sys_execve(frame, filename, NULL, NULL);
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
    struct thread *th = new_thread();
    struct process *proc = new_process(th);

    th->entry = new_userspace_entry;
    th->entry_arg = (void *)init_filename;
    th->cwd = fs_path("/bin");

    proc->mmap_base = USER_MMAP_BASE;
    proc->vm_root = vmm_fork(proc);

    th->state = TS_RUNNING;

    thread_enqueue(th);
}

static void deep_copy_fds(struct dmgr *child_fds, struct dmgr *parent_fds) {
    struct open_file *pfd, *cfd;
    for (int i = 0; i < parent_fds->cap; i++) {
        if ((pfd = dmgr_get(parent_fds, i)) == 0) { continue; }
        cfd = clone_open_file(pfd);
        dmgr_set(child_fds, i, cfd);
    }
}

sysret sys_create(const char *executable) {
    return -ETODO;
}

sysret sys_procstate(pid_t destination, enum procstate flags) {
    struct process *d_p = process_by_id(destination);
    struct process *p = running_process;

    if (flags & PS_COPYFDS) { deep_copy_fds(&d_p->fds, &p->fds); }

    if (flags & PS_SETRUN) {
        struct thread *th;
        th = list_head(struct thread, process_threads, &d_p->threads);
        thread_enqueue(th);
    }

    return 0;
}

static void finalizer_kthread(void *_) {
    while (true) {
        struct thread *th;

        disable_irqs();
        if (list_empty(&freeable_thread_queue)) {
            enable_irqs();
            thread_block();
        } else {
            th =
                list_pop_front(struct thread, freeable, &freeable_thread_queue);
            free_kernel_stack(th);
            free_thread_slot(th);
        }
    }
}

static int process_matches(pid_t wait_arg, struct process *proc) {
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

static void wake_waiting_parent_thread(void) {
    if (running_process->pid == 0) return;
    struct process *parent = running_process->parent;
    list_for_each(struct thread, parent_th, &parent->threads, process_threads) {
        if (parent_th->state != TS_WAIT) continue;
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

static void thread_cleanup(void) {
    list_remove(&running_thread->wait_node);
    list_remove(&running_thread->trace_node);
    list_remove(&running_thread->process_threads);
    list_remove(&running_thread->all_threads);
    list_remove(&running_thread->runnable);

    if (running_thread->wait_event) {
        drop_timer_event(running_thread->wait_event);
    }

    dmgr_drop(&threads, running_thread->tid);

    // running_thread->magic = 0;
}

static noreturn void do_thread_exit(int exit_status) {
    DEBUG_PRINTF("do_thread_exit(%i)\n", exit_status);
    struct thread *dead = running_thread;

    assert(dead->state != TS_DEAD);

    disable_irqs();
    thread_cleanup();

    if (list_empty(&running_process->threads)) {
        do_process_exit(exit_status);
        UNREACHABLE();
    }

    dead->state = TS_DEAD;

    make_freeable(dead);
    enable_irqs();
    thread_done();
}

static noreturn void do_process_exit(int exit_status) {
    struct thread *dead = running_thread;
    struct process *dead_proc = running_process;
    struct process *parent = dead_proc->parent;

    if (dead_proc->pid == 1) { panic("attempted to kill init!"); }

    assert(list_length(&dead_proc->threads) == 0);

    dead_proc->exit_status = exit_status + 1;

    wake_waiting_parent_thread();

    dead->state = TS_DEAD;
    make_freeable(dead);

    enable_irqs();
    thread_done();
}

noreturn sysret sys__exit(int exit_status) {
    kill_process(running_process, exit_status);
    UNREACHABLE();
}

/* TODO: add this to syscall table */
noreturn sysret sys_exit_thread(int exit_status) {
    do_thread_exit(exit_status);
}

noreturn void kthread_exit() {
    do_thread_exit(0);
}

sysret sys_fork(struct interrupt_frame *r) {
    DEBUG_PRINTF("sys_fork(%#lx)\n", r);

    if (running_process->pid == 0) { panic("Cannot fork() the kernel\n"); }

    struct thread *new_th = new_thread();
    struct process *new_proc = new_process(new_th);

    strncpy(new_proc->comm, running_process->comm, COMM_SIZE);
    new_proc->pgid = running_process->pgid;
    new_proc->uid = running_process->uid;
    new_proc->gid = running_process->gid;
    new_proc->mmap_base = running_process->mmap_base;

    // copy files to child
    deep_copy_fds(&new_proc->fds, &running_process->fds);

    new_th->user_sp = running_thread->user_sp;

    new_th->proc = new_proc;
    new_th->flags = running_thread->flags;
    new_th->cwd = running_thread->cwd;
    // new_th->flags &= ~TF_SYSCALL_TRACE;

    struct interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
    memcpy(frame, r, sizeof(interrupt_frame));
    FRAME_RETURN(frame) = 0;
    new_th->user_ctx = frame;
    new_th->flags |= TF_USER_CTX_VALID;

    new_th->kernel_ctx->__regs.ip = (uintptr_t)return_from_interrupt;
    new_th->kernel_ctx->__regs.sp = (uintptr_t)new_th->user_ctx;
    new_th->kernel_ctx->__regs.bp = (uintptr_t)new_th->user_ctx;

    new_proc->vm_root = vmm_fork(new_proc);
    new_th->state = TS_STARTED;

    thread_enqueue(new_th);
    sysret ret = new_proc->pid;
    return ret;
}

sysret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), void *new_stack,
                  void *arg, int flags) {
    DEBUG_PRINTF("sys_clone0(%#lx, %p, %p, %p, %i)\n", r, fn, new_stack, arg,
                 flags);

    if (running_process->pid == 0) {
        panic("Cannot clone() the kernel - you want kthread_create\n");
    }

    struct thread *new_th = new_thread();

    list_append(&running_process->threads, &new_th->process_threads);

    new_th->proc = running_process;
    new_th->flags = running_thread->flags;
    new_th->cwd = running_thread->cwd;

    struct interrupt_frame *frame = (interrupt_frame *)new_th->kstack - 1;
    memcpy(frame, r, sizeof(interrupt_frame));
    FRAME_RETURN(frame) = 0;
    new_th->user_ctx = frame;
    new_th->flags |= TF_USER_CTX_VALID;

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

sysret sys_getpid() {
    return running_process->pid;
}

sysret sys_gettid() {
    return running_thread->tid;
}

sysret sys_execve(struct interrupt_frame *frame, char *filename,
                  char *const argv[], char *const envp[]) {
    DEBUG_PRINTF("sys_execve(<frame>, \"%s\", <argv>, <envp>)\n", filename);

    struct file *file = fs_resolve_relative_path(running_thread->cwd, filename);
    if (!file) return -ENOENT;

    return do_execve(file, frame, filename, argv, envp);
}

sysret sys_execveat(struct interrupt_frame *frame, int dir_fd, char *filename,
                    char *const argv[], char *const envp[]) {
    struct open_file *ofd = dmgr_get(&running_process->fds, dir_fd);
    if (!ofd) return -EBADF;
    struct file *node = ofd->node;
    if (node->filetype != FT_DIRECTORY) return -ENOTDIR;

    struct file *file = fs_resolve_relative_path(node, filename);
    if (!file) return -ENOENT;

    return do_execve(file, frame, filename, argv, envp);
}

static void close_open_fd(void *fd) {
    struct open_file *ofd = fd;
    // printf("closing '%s'\n", ofd->node->filename);
    do_close_open_file(ofd);
}

static void destroy_child_process(struct process *proc) {
    // printf("destroying pid %i @ %p\n", proc->pid, proc);
    disable_irqs();
    assert(proc != running_process);
    assert(proc->exit_status);

    // ONE OF THESE IS WRONG
    assert(list_empty(&proc->threads));
    list_for_each(struct thread, th, &proc->threads, process_threads) {
        assert(list_node_null(&th->wait_node));
    }

    list_remove(&proc->siblings);

    struct process *init = process_by_id(1);
    if (!list_empty(&proc->children)) {
        list_for_each(struct process, child, &proc->children, siblings) {
            child->parent = init;
        }
        list_concat(&init->children, &proc->children);
    }

    dmgr_foreach(&proc->fds, close_open_fd);
    assert(list_length(&proc->threads) == 0);
    dmgr_free(&proc->fds);
    vmm_destroy_tree(proc->vm_root);
    free_process_slot(proc);
    enable_irqs();
}

static struct process *find_dead_child(pid_t query) {
    if (list_empty(&running_process->children)) return NULL;
    list_for_each(struct process, child, &running_process->children, siblings) {
        if (!process_matches(query, child)) continue;
        if (child->exit_status > 0) return child;
    }
    return NULL;
}

static struct thread *find_waiting_tracee(pid_t query) {
    if (list_empty(&running_thread->tracees)) return NULL;
    list_for_each(struct thread, th, &running_thread->tracees, trace_node) {
        if (query != 0 && query != th->tid) continue;
        if (th->state == TS_TRWAIT) return th;
    }
    return NULL;
}

sysret sys_waitpid(pid_t pid, int *status, enum wait_options options) {
    int exit_code;
    int found_pid;

    DEBUG_PRINTF("waitpid(%i, xx, xx)\n", pid);

    struct process *child = find_dead_child(pid);

    if (child) {
        exit_code = child->exit_status - 1;
        found_pid = child->pid;
        destroy_child_process(child);

        if (status) *status = exit_code;
        return found_pid;
    }

    struct thread *trace_th = find_waiting_tracee(pid);
    if (trace_th) {
        if (status) *status = trace_th->trace_report;
        return trace_th->tid;
    }

    if (list_empty(&running_process->children) &&
        list_empty(&running_thread->tracees)) {
        return -ECHILD;
    }

    if (options & WNOHANG) { return 0; }

    running_thread->wait_request = pid;
    running_thread->wait_result = 0;
    running_thread->wait_trace_result = 0;

    running_thread->state = TS_WAIT;

    while (running_thread->state == TS_WAIT) {
        thread_block();
        // rescheduled when a wait() comes in
        // see wake_waiting_parent_thread();
        // and trace_wake_tracer_with();
    }

    struct process *p = running_thread->wait_result;
    struct thread *t = running_thread->wait_trace_result;
    running_thread->wait_result = NULL;
    running_thread->wait_trace_result = NULL;

    if (p) {
        exit_code = p->exit_status - 1;
        found_pid = p->pid;
        destroy_child_process(p);
        if (status) *status = exit_code;
        return found_pid;
    }
    if (t) {
        if (status) *status = t->trace_report;
        return t->tid;
    }
    UNREACHABLE();
}

sysret sys_strace(int enable) {
    if (enable) {
        running_thread->flags |= TF_SYSCALL_TRACE;
    } else {
        running_thread->flags &= ~TF_SYSCALL_TRACE;
    }
    return enable;
}

void block_thread(list *blocked_threads) {
    DEBUG_PRINTF("** block %i\n", running_thread->tid);

    // assert(running_thread->wait_node.next == 0);

    disable_irqs();
    running_thread->state = TS_BLOCKED;
    list_append(blocked_threads, &running_thread->wait_node);
    enable_irqs();

    // whoever sets the thread blocking is responsible for bring it back
    thread_block();
}

void wake_thread(struct thread *t) {
    t->state = TS_RUNNING;
    assert(list_empty(&t->wait_node));
    thread_enqueue_at_front(t);
}

void wake_waitq_one(list *waitq) {
    struct thread *t;
    if (list_empty(waitq)) return;

    t = list_pop_front(struct thread, wait_node, waitq);
    wake_thread(t);
}

void wake_waitq_all(list *waitq) {
    if (list_empty(waitq)) return;

    list_for_each(struct thread, th, waitq, wait_node) {
        list_remove(&th->wait_node);
        wake_thread(th);
    }
}

sysret sys_yield(void) {
    thread_yield();
    return 0;
}

sysret sys_setpgid(int pid, int pgid) {
    if (pid != running_process->pid) { return -EPERM; }
    running_process->pgid = pgid;
    return 0;
}

sysret sys_exit_group(int exit_status) {
    // kill_process_group(running_process->pgid); // TODO
    kill_process(running_process, exit_status);
    UNREACHABLE();
}

static void handle_killed_condition() {
    if (running_thread->state == TS_DEAD) return;
    if (running_process->exit_intention) {
        do_thread_exit(running_process->exit_intention - 1);
    }
}

void kill_process(struct process *p, int reason) {
    struct thread *th, *tmp;

    if (list_empty(&p->threads)) return;

    p->exit_intention = reason + 1;

    list_for_each(struct thread, t, &p->threads, process_threads) {
        if (t == running_thread) continue;
        thread_enqueue(t);
    }

    if (p == running_process) { do_thread_exit(reason); }
}

void kill_pid(pid_t pid) {
    struct process *p = process_by_id(pid);
    if (p) kill_process(p, 0);
}

static void handle_stopped_condition() {
    while (running_thread->flags & TF_STOPPED) { thread_block(); }
}

__USED
static void print_thread(struct thread *th) {
    char *status;
    switch (th->state) {
    default: status = "?"; break;
    }

    printf("  t: %i %s%s%s\n", th->tid, "", status, " TODO");
}

__USED
static void print_process(void *p) {
    struct process *proc = p;

    if (proc->exit_status <= 0) {
        printf("pid %i: %s\n", proc->pid, proc->comm);
    } else {
        printf("pid %i: %s (defunct: %i)\n", proc->pid, proc->comm,
               proc->exit_status);
    }

    list_for_each(struct thread, th, &proc->threads, process_threads) {
        print_thread(th);
    }
}

sysret sys_top(int show_threads) {
    list_for_each(struct thread, th, &all_threads, all_threads) {
        printf("  %i:%i '%s'\n", th->proc->pid, th->tid, th->proc->comm);
    }
    return 0;
}

void unsleep_thread(struct thread *t) {
    t->wait_event = NULL;
    t->state = TS_RUNNING;
    thread_enqueue(t);
}

static void unsleep_thread_callback(void *t) {
    unsleep_thread(t);
}

void sleep_thread(int ms) {
    assert(running_thread->tid != 0);
    int ticks = milliseconds(ms);
    struct timer_event *te =
        insert_timer_event(ticks, unsleep_thread_callback, running_thread);
    running_thread->wait_event = te;
    running_thread->state = TS_SLEEP;
    thread_block();
}

sysret sys_sleepms(int ms) {
    sleep_thread(ms);
    return 0;
}

void thread_timer(void *_) {
    insert_timer_event(THREAD_TIME, thread_timer, NULL);
    thread_yield();
}

bool user_map(virt_addr_t base, virt_addr_t top) {
    struct mm_region *slot = NULL, *test;
    for (int i = 0; i < NREGIONS; i++) {
        test = &running_process->mm_regions[i];
        if (test->base == 0) {
            slot = test;
            break;
        }
    }

    if (!slot) return false;
    slot->base = base;
    slot->top = top;
    slot->flags = 0;
    slot->file = 0;

    vmm_create_unbacked_range(base, top - base, PAGE_WRITEABLE | PAGE_USERMODE);
    return true;
}

void proc_threads(struct open_file *ofd, void *_) {
    list_for_each(struct thread, th, &all_threads, all_threads) {
        struct process *p = th->proc;
        proc_sprintf(ofd, "%i %i %s\n", th->tid, p->pid, p->comm);
    }
}
