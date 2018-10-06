
// #define DEBUG
#include "thread.h"
#include <basic.h>
#include <debug.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include <string.h>
#include <panic.h>
#include <debug.h>
#include <vector.h>
#include <dmgr.h>
#include <arch/cpu.h>
#include <syscall.h>
#include <syscalls.h>
#include <mutex.h>
#include <vmm.h>

// These seem kinda strange to have in thread.c...:
#include <fs/vfs.h>
#include <fs/tarfs.h>
#include <elf.h>
#include <arch/x86/pit.h>

extern uintptr_t boot_pt_root;

struct queue runnable_thread_queue = { 0 };

kmutex process_lock = KMUTEX_INIT;
// struct vector processes;
struct dmgr processes;
struct dmgr threads;

struct process proc_zero = {
    .pid = 0,
    .is_kernel = true,
    .vm_root = (uintptr_t)&boot_pt_root,
    .parent = 0,
    .thread_count = 1,
    .blocked_threads = {0},
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
    .tid = 0,
    .running = true,
    .strace = false,
    .stack = &boot_kernel_stack,
    .state = THREAD_RUNNING,
    .pid = 0,
};

struct process* running_process = &proc_zero;
struct thread* running_thread = &thread_zero;

void threads_init() {
    DEBUG_PRINTF("init_threads()\n");

    await_mutex(&process_lock);

    dmgr_init(&processes);
    dmgr_init(&threads);
    printf("threads: thread data at %p\n", processes.data);

    dmgr_insert(&processes, &proc_zero);
    dmgr_insert(&threads, &thread_zero);

    release_mutex(&process_lock);
}

void set_kernel_stack(void *stack_top) {
    extern uintptr_t *kernel_stack;
    *&kernel_stack = stack_top;
}

void enqueue_thread(struct thread* th) {
    struct queue_object* tq =
        malloc(sizeof(struct queue_object) + sizeof(struct thread*));
    *(struct thread**)&tq->data = th;
    queue_enqueue(&runnable_thread_queue, tq);
}

void enqueue_thread_at_front(struct thread* th) {
    struct queue_object* tq =
        malloc(sizeof(struct queue_object) + sizeof(struct thread*));
    *(struct thread**)&tq->data = th;
    queue_enqueue_at_front(&runnable_thread_queue, tq);
}

void enqueue_thread_inplace(struct thread* th, struct queue_object* memory) {
    assert(memory, "need memory to construct inplace");

    *(struct thread**)&memory->data = th;
    queue_enqueue(&runnable_thread_queue, memory);
}

// currently in boot.asm
uintptr_t read_ip();

void switch_thread(int reason) {
    if (process_lock) {
        DEBUG_PRINTF("blocked from switching by the process lock\n");
        pit_create_oneshot(100);
        return; // cannot switch now
    }
    disable_irqs();

    // printf("there are %i threads waiting to be run\n",
    //         queue_count(&runnable_thread_queue));

    struct thread* to = NULL;
    struct queue_object* qo = queue_dequeue(&runnable_thread_queue);

    if (qo) {
        to = *(struct thread**)&qo->data;

        if (running_thread->tid != 0 &&
            running_thread->state == THREAD_RUNNING) {

            enqueue_thread_inplace(running_thread, qo);
        }
    } else {
        if (reason != SW_BLOCK &&
            running_thread->state == THREAD_RUNNING) {
            // this thread ran out of time, but no one else is ready to be run
            // so give it another time slot.

            enable_irqs();
            pit_create_oneshot(10003); // give process max 10ms to run
            return;
        } else {
            to = &thread_zero;
        }
    }

    DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

    struct process *to_proc = dmgr_get(&processes, to->pid);
    set_kernel_stack(to->stack);
    if (!to_proc->is_kernel) {
        // TODO: if we switched to the kernel then back to the same
        // task we can also skip this
        set_vm_root(to_proc->vm_root);
    } /* else {
        printf("switching to a kernel thread, no vmm switch\n");
    } */

#if X86_64
    asm volatile ("mov %%rsp, %0" : "=r"(running_thread->sp));
    asm volatile ("mov %%rbp, %0" : "=r"(running_thread->bp));
#elif I686
    asm volatile ("mov %%esp, %0" : "=r"(running_thread->sp));
    asm volatile ("mov %%ebp, %0" : "=r"(running_thread->bp));
#endif

    uintptr_t ip = read_ip();
    if (ip == 0x99) {
        // task switch completed and we have returned to this one

        if (running_thread->tid == 0) {
            pit_ignore();
        } else {
            pit_create_oneshot(10002); // give process max 10ms to run
        }

        return;
    }
    running_thread->ip = ip;

    running_process = to_proc;
    running_thread = to;

    if (running_thread->tid == 0) {
        pit_ignore();
    } else {
        pit_create_oneshot(10001); // give process max 10ms to run
    }

    enable_irqs();

#if X86_64
    asm volatile (
        "mov %0, %%rbx\n\t"
        "mov %1, %%rsp\n\t"
        "mov %2, %%rbp\n\t"

        // This makes read_ip return 0x99 when we switch back to it
        "mov $0x99, %%rax\n\t"

        "jmp *%%rbx"
        :
        : "r"(to->ip), "r"(to->sp), "r"(to->bp)
        : "%rbx", "%rsp", "%rax"
    );
#elif I686
    asm volatile (
        "mov %0, %%ebx\n\t"
        "mov %1, %%esp\n\t"
        "mov %2, %%ebp\n\t"

        // This makes read_ip return 0x99 when we switch back to it
        "mov $0x99, %%eax\n\t"

        "jmp *%%ebx"
        :
        : "r"(to->ip), "r"(to->sp), "r"(to->bp)
        : "%ebx", "%esp", "%eax"
    );
#endif
}

// TODO: move this
#if X86_64
# define STACKS_START 0xffffffff85000000
#elif I686
# define STACKS_START 0x85000000
#endif
// TODO: move this
#if X86_64
# define HEAP_START 0xffffff0000000000
#elif I686
# define HEAP_START 0xc0000000
#endif

void* new_kernel_stack() {
    static uintptr_t this_stack = STACKS_START;
    // leave 1 page unmapped for guard
    this_stack += PAGE_SIZE;
    // 8k stack
    vmm_create(this_stack, PAGE_WRITEABLE);
    this_stack += PAGE_SIZE;
    vmm_create(this_stack, PAGE_WRITEABLE);
    this_stack += PAGE_SIZE;
    DEBUG_PRINTF("new kernel stack at (top): %p\n", this_stack);
    return (void*)this_stack;
}

void new_kernel_thread(uintptr_t entrypoint) {
    DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);
    struct thread *th = malloc(sizeof(struct thread));

    int new_tid = dmgr_insert(&threads, th);

    memset(th, 0, sizeof(struct thread));

    await_mutex(&process_lock);

    th->stack = (char*)new_kernel_stack() - 8;

    th->tid = new_tid;
    th->bp = th->stack; // - sizeof(struct interrupt_frame);
    th->sp = th->bp;
    th->ip = entrypoint;
    th->pid = 0;

    proc_zero.thread_count += 1;
    release_mutex(&process_lock);

    th->state = THREAD_RUNNING;

    enqueue_thread(th);
}

void return_from_interrupt();

// TODO: move this
#if X86_64
# define USER_STACK 0x7FFFFF000000
# define USER_ARGV 0x7FFFFF001000
# define USER_ENVP 0x7FFFFF002000
#elif I686
# define USER_STACK 0x7FFF0000
# define USER_ARGV 0x7FFF1000
# define USER_ENVP 0x7FFF2000
#endif

void new_user_process(uintptr_t entrypoint) {
    DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
    struct process* proc = malloc(sizeof(struct process));
    struct thread *th = malloc(sizeof(struct thread));

    memset(proc, 0, sizeof(struct process));
    memset(th, 0, sizeof(struct thread));

    proc->pid = -1;
    proc->is_kernel = false;
    proc->parent = 0;
    proc->thread_count = 1;

    await_mutex(&process_lock);
    pid_t pid = dmgr_insert(&processes, proc);
    int new_tid = dmgr_insert(&threads, th);
    running_process = proc;

    proc->pid = pid;
    vec_init(&proc->fds, size_t);
    vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stdin (0)
    vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stdout (1)
    vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stderr (2)

    th->tid = new_tid;
    th->stack = (char*)new_kernel_stack() - 8;
    th->bp = th->stack - sizeof(struct interrupt_frame);
    th->sp = th->bp;
    th->ip = (uintptr_t)return_from_interrupt;
    th->pid = proc->pid;
    th->strace = false;

#if X86_64
    struct interrupt_frame *frame = th->sp;
#elif I686
    // I686 has to push a parameter to the C interrupt shim,
    // so the first thing return_from_interrupt does is restore
    // the stack.  This needs to start 4 higher to be compatible with
    // that ABI.
    struct interrupt_frame *frame = (void*)((char*)th->sp + 4);
#endif
    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame_set(frame, IP, entrypoint);
    frame_set(frame, SP, USER_STACK - 16);
    frame_set(frame, BP, USER_STACK - 16);

    vmm_create_unbacked_range(
            USER_STACK - 0x100000, 0x100000, PAGE_USERMODE | PAGE_WRITEABLE);
    vmm_create_unbacked_range(
            USER_ARGV, 0x2000, PAGE_USERMODE | PAGE_WRITEABLE);

// TODO: x86ism
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame_set(frame, FLAGS, INTERRUPT_ENABLE);

    proc->vm_root = vmm_fork();
    release_mutex(&process_lock);

    th->state = THREAD_RUNNING;

    enqueue_thread(th);
    switch_thread(SW_BLOCK);
}

noreturn void do_thread_exit(int exit_status, int thread_state) {
    DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, thread_state);
    running_thread->state = thread_state;
    running_thread->exit_status = exit_status;

    running_process->thread_count -= 1;
    assert(running_process->thread_count >= 0,
            "killed more threads than exist...");

    dmgr_drop(&threads, running_thread->tid);

    if (running_process->thread_count == 0) {
        running_process->exit_status = exit_status;

        if (!running_process->refcnt)
            dmgr_drop(&processes, running_process->pid);

        wake_blocked_threads(&running_process->blocked_threads);
        // TODO: signal parent proc of death
    }
    switch_thread(SW_YIELD);

    panic("How even did we get here\n");
}

noreturn struct syscall_ret sys_exit(int exit_status) {
    do_thread_exit(exit_status, THREAD_DONE);
}

noreturn void kill_running_thread(int exit_status) {
    do_thread_exit(exit_status, THREAD_KILLED_FOR_VIOLATION);
}

struct syscall_ret sys_fork(struct interrupt_frame* r) {
    DEBUG_PRINTF("sys_fork(%#lx)\n", r);

    if (running_process->is_kernel) {
        panic("Cannot fork() the kernel\n");
    }

    struct process* new_proc = malloc(sizeof(struct process));
    struct thread* new_th = malloc(sizeof(struct thread));

    memset(new_proc, 0, sizeof(struct process));
    memset(new_th, 0, sizeof(struct thread));

    new_proc->pid = -1;
    new_proc->is_kernel = false;
    new_proc->parent = running_process->pid;
    new_proc->thread_count = 1;

    // copy files to child
    vec_init_copy(&new_proc->fds, &running_process->fds);

    await_mutex(&process_lock);
    pid_t new_pid = dmgr_insert(&processes, new_proc);
    int new_tid = dmgr_insert(&threads, new_th);
    running_process = new_proc;
    new_proc->pid = new_pid;

    new_th->tid = new_tid;
    new_th->stack = new_kernel_stack();
#if I686
    // see below
    new_th->stack -= 4;
#endif
    new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
    new_th->sp = new_th->bp;
    new_th->ip = (uintptr_t)return_from_interrupt;
    new_th->pid = new_pid;
    new_th->strace = 0;

#if X86_64
    struct interrupt_frame* frame = new_th->sp;
#elif I686
    // I686 has to push a parameter to the C interrupt shim,
    // so the first thing return_from_interrupt does is restore
    // the stack.  This needs to start 4 higher to be compatible with
    // that ABI.
    struct interrupt_frame* frame = (void*)((char*)new_th->sp + 4);
#endif
    memcpy(frame, r, sizeof(struct interrupt_frame));
    frame_set(frame, RET_VAL, 0);
    frame_set(frame, RET_ERR, 0);

    new_proc->vm_root = vmm_fork();
    new_th->state = THREAD_RUNNING;
    
    enqueue_thread(new_th);

    struct syscall_ret ret = { new_proc->pid, 0 };
    release_mutex(&process_lock);

    return ret;
}

struct syscall_ret sys_getpid() {
    struct syscall_ret ret = { running_process->pid, 0 };
    return ret;
}

struct syscall_ret sys_gettid() {
    struct syscall_ret ret = { running_thread->tid, 0 };
    return ret;
}

extern struct tar_header* initfs;

struct syscall_ret sys_execve(
        struct interrupt_frame *frame, char *filename,
        char **argv, char **envp) {

    DEBUG_PRINTF("sys_execve(stuff)\n");

    if (running_process->is_kernel) {
        panic("cannot execve() the kernel\n");
    }

    struct syscall_ret ret = { 0, 0 };

    void* file = tarfs_get_file(initfs, filename);

    if (!file) {
        // Bad file, cannot proceed
        ret.error = ENOENT;
        return ret;
    }

    Elf64_Ehdr* elf = file;

    if (!elf_verify(elf)) {
        // Bad file, cannot proceed
        ret.error = ENOEXEC;
        return ret;
    }
    // TODO: ensure we have enough unbacked space / backed space ?
    elf_load(elf);

    memset(frame, 0, sizeof(struct interrupt_frame));
    // TODO: x86ism
    frame->ds = 0x18 | 3;
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame_set(frame, IP, (uintptr_t)elf->e_entry);
    frame_set(frame, FLAGS, INTERRUPT_ENABLE);

    // on I686, arguments are passed above the initial stack pointer
    // so give them some space.  This may not be needed on other
    // platforms, but it's ok for the moment
    frame_set(frame, SP, USER_STACK - 16);
    frame_set(frame, BP, USER_STACK - 16);

    // pretty sure I shouldn't use the environment area for argv...
    char* argument_data = (void*)USER_ENVP;
    char** user_argv = (void*)USER_ARGV;

    size_t argc = 0;
    user_argv[argc++] = argument_data;
    argument_data = strcpy(argument_data, filename);
    argument_data += 1;
    while (*argv) {
        user_argv[argc++] = argument_data;
        argument_data = strcpy(argument_data, *argv);
        argument_data += 1;
        argv += 1;
    }

    frame_set(frame, ARGC, argc);
    frame_set(frame, ARGV, (uintptr_t)user_argv);

    return ret; // value goes nowhere since ip moved.
}

struct syscall_ret sys_wait4(pid_t process) {
    // 
    // I misunderstood this syscall
    // this is actually a standard thing and I implemented a
    // very minimal version of it that is far from compatible
    // with the real one.
    //
    // I will likely fix this in the future, for the moment
    // I am disabling the syscall that actually calls this and
    // marking it deprecated.  The functionality is replaced by
    // sys_waitpid.  At some point in the future this will be
    // updated for compatability with the real wait4.
    // 
    struct syscall_ret ret = { 0, 0 };
    while (true) {
        await_mutex(&process_lock);
        struct process *proc = dmgr_get(&processes, process);

        if (proc->thread_count) {
            release_mutex(&process_lock);
            asm volatile ("hlt");
            continue;
        }
        ret.value = proc->exit_status;
        release_mutex(&process_lock);

        return ret;
        // could this be structured better?
    }
}

struct syscall_ret sys_waitpid(pid_t process, int* status, int options) {
    if (process <= 0) {
        // TODO support 'any child' or groups
        RETURN_ERROR(EINVAL);
    }
    struct process* proc = dmgr_get(&processes, process);
    if (!proc) {
        RETURN_ERROR(ECHILD);
    }
    proc->refcnt++;

    // TODO: permissions checking.
    while (proc->thread_count) {
        if (options & WNOHANG)
            RETURN_VALUE(0);

        block_thread(&proc->blocked_threads);
    }
    *status = proc->exit_status;

    proc->refcnt--;
    if (!proc->refcnt)
        dmgr_drop(&processes, process);
    
    RETURN_VALUE(process);
}

struct syscall_ret sys_strace(bool enable) {
    struct syscall_ret ret = { 0, 0 };
    running_thread->strace = enable;
    return ret;
}

void block_thread(struct queue* blocked_threads) {
    struct queue_object* qo =
        malloc(sizeof(struct queue_object) + sizeof(struct thread*));
    *(struct thread**)&qo->data = running_thread;

    DEBUG_PRINTF("** block %i\n", running_thread->tid);

    running_thread->state = THREAD_BLOCKED;
    queue_enqueue(blocked_threads, qo);

    // whoever sets the thread blocking is responsible for bring it back
    pit_ignore();
    switch_thread(SW_BLOCK);
}

void wake_blocked_threads(struct queue* blocked_threads) {
    struct queue_object* qo = NULL;
    struct thread* last_thread = NULL;
    int wake_count = 0;

    while ((qo = queue_dequeue(blocked_threads))) {
        struct thread* th = *(struct thread**)&qo->data;

        DEBUG_PRINTF("** wake %i\n", th->tid);

        th->state = THREAD_RUNNING;
        enqueue_thread(th);

        free(qo);

        wake_count += 1;
    }
    if (wake_count) {
        switch_thread(SW_YIELD);
    }
}

void requeue_next_blocked_thread(struct queue* blocked_threads) {
}

struct syscall_ret sys_yield(void) {
    switch_thread(SW_YIELD);
    RETURN_VALUE(0);
}

