
// #define DEBUG
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
#include <arch/cpu.h>
#include <syscall.h>
#include <syscalls.h>
#include <mutex.h>
#include <vmm.h>
// These seem kinda strange to have in thread.c...:
#include <fs/vfs.h>
#include <fs/tarfs.h>
#include <elf.h>
#include "thread.h"

extern uintptr_t boot_pt_root;

struct thread_queue *runnable_threads = NULL;
struct thread_queue *runnable_threads_tail = NULL;

int top_pid_tid = 1;

kmutex process_lock = KMUTEX_INIT;
struct vector process_list;

struct process proc_zero = {
    .pid = 0,
    .is_kernel = true,
    .vm_root = (uintptr_t)&boot_pt_root,
    .parent = 0,
    .thread_count = 1,
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
    vec_init(&process_list, struct process);
    printf("threads: thread data at %p\n", process_list.data);
    vec_push(&process_list, &proc_zero);
    running_process = vec_get(&process_list, running_thread->pid);
    release_mutex(&process_lock);
    // create a vector for threads too?
}

void set_kernel_stack(void *stack_top) {
    extern uintptr_t *kernel_stack;
    *&kernel_stack = stack_top;
}

void enqueue_thread(struct thread *th) {
    if (runnable_threads == NULL) {
        runnable_threads = malloc(sizeof(struct thread_queue));
        runnable_threads_tail = runnable_threads;
    } else {
        runnable_threads_tail->next = malloc(sizeof(struct thread_queue));
        runnable_threads_tail = runnable_threads_tail->next;
    }

    runnable_threads_tail->sched = th;
    runnable_threads_tail->next = NULL;
}

void enqueue_thread_inplace(struct thread* th, struct thread_queue* memory) {
    assert(memory, "need memory to construct inplace");
    if (runnable_threads == NULL) {
        runnable_threads = memory;
        runnable_threads_tail = runnable_threads;
    } else {
        runnable_threads_tail->next = memory;
        runnable_threads_tail = runnable_threads_tail->next;
    }

    runnable_threads_tail->sched = th;
    runnable_threads_tail->next = NULL;
}

// currently in boot.asm
uintptr_t read_ip();

void switch_thread(struct thread *to) {
    if (process_lock) {
        printf("blocked from switching by the process lock\n");
        return; // cannot switch now
    }
    asm volatile ("cli"); // can't be interrupted here

    struct thread_queue* tmp = runnable_threads;
    if (tmp && do_debug) {
        printf("the runnable queue is: ");
        for (; tmp != NULL; tmp = tmp->next) {
            printf("(%i, %i) -> ", tmp->sched->pid, tmp->sched->tid);
        }
        printf("NULL");
    }

    struct thread_queue* old;

    if (to == NULL) {
        do {
            if (!runnable_threads) {
                return;  // nothing to do
            }
            to = runnable_threads->sched;
            old = runnable_threads;
            runnable_threads = runnable_threads->next;
        } while (to->state != THREAD_RUNNING);

        if (to == running_thread) {
            return;  // switching to this thread is a no-op
        }

        // thread switch debugging:
        DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

        if (running_thread != &thread_zero) {
            enqueue_thread_inplace(running_thread, old);
        }
    } else {
        // open question:
        // how much does this do if you specify a next thread?
        // currently I don't even support this.
        // should I continue to not?
    }

    struct process *to_proc = vec_get(&process_list, to->pid);
    set_kernel_stack(to->stack);
    set_vm_root(to_proc->vm_root);

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
        return;
    }
    running_thread->ip = ip;

    running_process = to_proc;
    running_thread = to;

    asm volatile ("sti");

#if X86_64
    asm volatile (
        "mov %0, %%rbx\n\t"
        "mov %1, %%rsp\n\t"
        "mov %2, %%rbp\n\t"

        // This makes read_ip return 0x99 when we switch back to it
        "mov $0x99, %%rax\n\t"

        "jmp *%%rbx"
        :: "r"(to->ip), "r"(to->sp), "r"(to->bp)
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
        :: "r"(to->ip), "r"(to->sp), "r"(to->bp)
        : "%ebx", "%esp", "%eax"
    );
#endif
}

noreturn void kill_running_thread(int exit_status) {
    DEBUG_PRINTF("kill_running_thread(%i)\n", exit_status);
    // COPYPASTE from sys_exit

    running_thread->state = THREAD_KILLED_FOR_VIOLATION;
    running_thread->exit_status = exit_status;

    running_process->thread_count -= 1;
    assert(running_process->thread_count >= 0, "killed more threads than exist...");

    if (running_process->thread_count == 0) {
        running_process->exit_status = exit_status;
        // TODO: signal parent proc of death
    }

    while (true) {
        asm volatile ("hlt");
    }
}

// TODO: move this
#if X86_64
# define STACKS_START 0xffffffff85000000
#elif I686
# define STACKS_START 0x85000000
#endif
// TODO: move this
#if X86_64
# define HEAP_START 0xffffffff88000000
#elif I686
# define HEAP_START 0x88000000
#endif

void* new_kernel_stack() {
    static uintptr_t this_stack = STACKS_START;
    // leave 1 page unmapped for guard
    this_stack += PAGE_SIZE;
    // 8k stack
    vmm_create_unbacked(this_stack, PAGE_WRITEABLE | PAGE_GLOBAL);
    this_stack += PAGE_SIZE;
    vmm_create_unbacked(this_stack, PAGE_WRITEABLE | PAGE_GLOBAL);
    this_stack += PAGE_SIZE;
    DEBUG_PRINTF("new kernel stack at (top): %p\n", this_stack);
    if (this_stack >= HEAP_START) {
        printf("kernel stacks are going to overwrite the heap\n");
        printf("either move this or that or write that virtual\n");
        printf("memory allocator you were thinking about\n");
        panic();
    }
    return (void*)this_stack;
}

void new_kernel_thread(uintptr_t entrypoint) {
    DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);
    struct thread *th = malloc(sizeof(struct thread));
    memset(th, 0, sizeof(struct thread));

    await_mutex(&process_lock);
    struct process *proc_zero = vec_get(&process_list, 0);

    th->pid = 0;
    th->stack = new_kernel_stack();

    th->ip = entrypoint;
    th->sp = th->stack;
    th->bp = th->sp;

    th->tid = top_pid_tid++;

    proc_zero->thread_count += 1;
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
# define USER_ENVP 0x7FFF1000
#endif

void new_user_process(uintptr_t entrypoint) {
    DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
    struct process proc;
    struct thread *th = malloc(sizeof(struct thread));

    memset(th, 0, sizeof(struct thread));

    proc.pid = -1;
    proc.is_kernel = false;
    proc.parent = 0;
    proc.thread_count = 1;

    await_mutex(&process_lock);
    pid_t pid = vec_push(&process_list, &proc);
    running_process = vec_get(&process_list, running_thread->pid);
    struct process *pproc = vec_get(&process_list, pid);
    pproc->pid = pid;
    vec_init(&pproc->fds, size_t);
    vec_push_value(&pproc->fds, 1); // DEV_SERIAL -> stdin (0)
    vec_push_value(&pproc->fds, 1); // DEV_SERIAL -> stdout (1)
    vec_push_value(&pproc->fds, 1); // DEV_SERIAL -> stderr (2)

    th->tid = top_pid_tid++;
    th->stack = new_kernel_stack();
    th->bp = th->stack - sizeof(struct interrupt_frame);
    th->sp = th->bp;
    th->ip = (uintptr_t)return_from_interrupt;
    th->pid = pproc->pid;
    // th->strace = true;


    struct interrupt_frame *frame = th->sp;
    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame_set(frame, IP, entrypoint);
    frame_set(frame, SP, USER_STACK);

    // 0x7FFFFF000000 is explicitly unallocated so stack underflow traps.
    vmm_create_unbacked_range(USER_STACK - 0x100000, 0x100000, PAGE_USERMODE | PAGE_WRITEABLE);
    // following pages allocated for argv and envp during future exec's:
    vmm_create_unbacked_range(USER_ARGV, 0x2000, PAGE_USERMODE | PAGE_WRITEABLE);

// TODO: x86ism
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame_set(frame, FLAGS, INTERRUPT_ENABLE);

    pproc->vm_root = vmm_fork();
    release_mutex(&process_lock);

    th->state = THREAD_RUNNING;

    printf("new_user_process created this:\n");
    print_registers(frame);

    enqueue_thread(th);
}


noreturn struct syscall_ret sys_exit(int exit_status) {
    DEBUG_PRINTF("sys_exit(%i)\n", exit_status);
    running_thread->state = THREAD_DONE; // TODO: this might leak
    running_thread->exit_status = exit_status;

    running_process->thread_count -= 1;
    assert(running_process->thread_count >= 0, "killed more threads than exist...");

    if (running_process->thread_count == 0) {
        running_process->exit_status = exit_status;
        // TODO: signal parent proc of death
    }

    while (true) {
        asm volatile ("hlt");
    }
}

struct syscall_ret sys_fork(struct interrupt_frame *r) {
    DEBUG_PRINTF("sys_fork(%#lx)\n", r);

    if (running_process->is_kernel) {
        panic("Cannot fork() the kernel\n");
    }

    struct process new_proc;
    struct thread *new_th = malloc(sizeof(struct thread));

    new_proc.pid = -1;
    new_proc.is_kernel = false;
    new_proc.parent = running_process->pid;
    new_proc.thread_count = 1;

    await_mutex(&process_lock);
    pid_t new_pid = vec_push(&process_list, &new_proc);
    running_process = vec_get(&process_list, running_thread->pid);
    struct process *pnew_proc = vec_get(&process_list, new_pid);
    pnew_proc->pid = new_pid;
    vec_init_copy(&pnew_proc->fds, &running_process->fds); // copy files to child

    new_th->tid = top_pid_tid++;
    new_th->stack = new_kernel_stack();
    new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
    new_th->sp = new_th->bp;
    new_th->ip = (uintptr_t)return_from_interrupt;
    new_th->pid = new_pid;
    new_th->strace = 0;

    struct interrupt_frame *frame = new_th->sp;
    memcpy(frame, r, sizeof(struct interrupt_frame));
    frame_set(frame, RET_VAL, 0);
    frame_set(frame, RET_ERR, 0);

    pnew_proc->vm_root = vmm_fork();
    new_th->state = THREAD_RUNNING;
    
    enqueue_thread(new_th);

    struct syscall_ret ret = { pnew_proc->pid, 0 };
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

struct tar_header;
void *tarfs_get_file(struct tar_header *, char *);
extern void *initfs;

struct syscall_ret sys_execve(struct interrupt_frame *frame, char *filename, char **argv, char **envp) {
    DEBUG_PRINTF("sys_execve(stuff)\n");

    if (running_process->is_kernel) {
        panic("cannot execve() the kernel\n");
    }

    struct syscall_ret ret = { 0, 0 };

    void *file = tarfs_get_file(initfs, filename);

    if (!file) {
        // Bad file, cannot proceed
        ret.error = ENOENT;
        return ret;
    }

    Elf64_Ehdr *elf = file;

    if (!elf_verify(elf)) {
        // Bad file, cannot proceed
        ret.error = ENOEXEC;
        return ret;
    }
    // TODO: ensure we have enough unbacked space / backed space ?
    elf_load(elf);

    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame_set(frame, IP, (uintptr_t)elf->e_entry);
    frame_set(frame, SP, USER_STACK);
    frame_set(frame, BP, 0);
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame_set(frame, FLAGS, INTERRUPT_ENABLE);

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
        struct process *proc = vec_get(&process_list, process);

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
    struct syscall_ret ret = { 0, 0 };

    if (process <= 0) {
        ret.error = EINVAL; // TODO support 'any child' or groups
        return ret;
    }

    while (true) {
        await_mutex(&process_lock);
        struct process *proc = vec_get(&process_list, process);

        if (!proc) {
            release_mutex(&process_lock);
            ret.error = ECHILD;
            return ret;
        }

        // TODO: permissions checking.
        if (proc->thread_count) {
            release_mutex(&process_lock);
            if (options & WNOHANG)  return ret;
            asm volatile ("hlt");
            continue;
        }

        *status = proc->exit_status;
        // vec_free(&proc->fds);
        release_mutex(&process_lock);

        ret.value = process;
        return ret;
    }
}

struct syscall_ret sys_strace(bool enable) {
    struct syscall_ret ret = { 0, 0 };
    running_thread->strace = enable;
    return ret;
}

