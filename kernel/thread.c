
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include <string.h>
#include <panic.h>
#include <debug.h>
#include <vector.h>
#include <arch/x86/cpu.h>
#include <arch/x86/interrupt.h> // temp for print_registers
#include <syscall.h>
#include <syscalls.h>
#include <vmm.h>
// These seem kinda strange to have in thread.c...:
#include <fs/vfs.h>
#include <fs/tarfs.h>
#include <elf.h>

#include <arch/x86/cpu.h>

#include "thread.h"

extern uintptr_t boot_pml4;

struct thread_queue *runnable_threads = NULL;
struct thread_queue *runnable_threads_tail = NULL;

int top_pid_tid = 1;

struct vector process_list = {0};

struct process proc_zero = {
    .pid = 0,
    .is_kernel = true,
    .vm_root = (uintptr_t)&boot_pml4,
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

void init_threads() {
    vec_init(&process_list, struct process);
    printf("threads: thread data at %p\n", process_list.data);
    vec_push(&process_list, &proc_zero);
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

void enqueue_thread_inplace(struct thread *th, struct thread_queue* memory) {
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
uintptr_t read_rip();

void switch_thread(struct thread *to) {
    struct thread_queue* old;

    if (to == NULL) {
        do {
            if (!runnable_threads) {
                to = running_thread;
                break;
            }
            to = runnable_threads->sched;
            old = runnable_threads;
            runnable_threads = runnable_threads->next;
        } while (!(to->state == THREAD_RUNNING));

        if (to == running_thread) {
            return; // switching to this thread is a no-op
        }

        // thread switch debugging:
        // printf("[am %i, to %i]\n", running_thread->tid, to->tid);

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
    set_kernel_stack(to->stack + STACK_SIZE);
    set_vm_root(to_proc->vm_root);

    asm volatile ("mov %%rsp, %0" : "=r"(running_thread->rsp));
    asm volatile ("mov %%rbp, %0" : "=r"(running_thread->rbp));
    uintptr_t rip = read_rip();
    if (rip == 0x99) {
        // task switch completed and we have returned to this one
        return;
    }
    running_thread->rip = (void *)rip;

    running_process = to_proc;
    running_thread = to;

    asm volatile (
            "mov %0, %%rbx\n\t"
            "mov %1, %%rsp\n\t"
            "mov %2, %%rbp\n\t"

            // This makes read_rip return 0x99 when we switch back to it
            "mov $0x99, %%rax\n\t"

            "jmp *%%rbx"
            :: "r"(to->rip), "r"(to->rsp), "r"(to->rbp)
            : "%rbx", "%rsp", "%rax"
    );
}

void kill_running_thread(int exit_status) {
    // COPYPASTE from sys_exit

    running_thread->state = THREAD_KILLED_FOR_VIOLATION;
    running_thread->exit_status = exit_status;

    struct process *proc = vec_get(&process_list, running_thread->pid);
    proc->thread_count -= 1;
    assert(proc->thread_count >= 0, "killed more threads than exist...");

    if (proc->thread_count == 0) {
        proc->exit_status = exit_status;
        // TODO: signal parent proc of death
    }

    while (true) {
        asm volatile ("hlt");
    }
    __builtin_unreachable();
}

void new_kernel_thread(void *entrypoint) {
    struct thread *th = malloc(sizeof(struct thread));
    memset(th, 0, sizeof(struct thread));
    struct process *proc_zero = vec_get(&process_list, 0);

    th->pid = 0;
    th->stack = malloc(STACK_SIZE);

    th->rip = entrypoint;
    th->rsp = th->stack + STACK_SIZE;
    th->rbp = th->rsp;

    th->tid = top_pid_tid++;

    proc_zero->thread_count += 1;
    th->state = THREAD_RUNNING;

    enqueue_thread(th);
}

void return_from_interrupt();

void new_user_process(void *entrypoint) {
    struct process proc;
    struct thread *th = malloc(sizeof(struct thread));

    memset(th, 0, sizeof(struct thread));

    proc.pid = -1;
    proc.is_kernel = false;
    proc.parent = 0;
    proc.thread_count = 1;

    pid_t pid = vec_push(&process_list, &proc);
    struct process *pproc = vec_get(&process_list, pid);
    pproc->pid = pid;
    vec_init(&pproc->fds, size_t);
    vec_push_value(&pproc->fds, 4); // DEV_SERIAL -> stdin (0)
    vec_push_value(&pproc->fds, 1); // DEV_STDOUT -> stdout (1)
    vec_push_value(&pproc->fds, 1); // DEV_STDOUT -> stderr (2)

    th->tid = top_pid_tid++;
    th->stack = malloc(STACK_SIZE);
    th->rbp = th->stack + STACK_SIZE - sizeof(struct interrupt_frame);
    th->rsp = th->rbp;
    th->rip = return_from_interrupt;
    th->pid = pproc->pid;
    // th->strace = true;

    struct interrupt_frame *frame = th->rsp;
    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame->rip = (uintptr_t)entrypoint;
    frame->user_rsp = 0x7FFFFF000000;

    // 0x7FFFFF000000 is explicitly unallocated so stack underflow traps.
    vmm_create_unbacked_range(0x7FFFFF000000 - 0x100000, 0x100000, PAGE_USERMODE | PAGE_WRITEABLE);
    // following pages allocated for argv and envp during future exec's:
    vmm_create_unbacked_range(0x7FFFFF001000, 0x2000, PAGE_USERMODE | PAGE_WRITEABLE);
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame->rflags = 0x200;

    pproc->vm_root = vmm_fork();
    th->state = THREAD_RUNNING;

    // print_registers(frame);

    enqueue_thread(th);
}


struct syscall_ret sys_exit(int exit_status) {
    running_thread->state = THREAD_DONE; // TODO: this might leak
    running_thread->exit_status = exit_status;

    struct process *proc = vec_get(&process_list, running_thread->pid);
    proc->thread_count -= 1;
    assert(proc->thread_count >= 0, "killed more threads than exist...");

    if (proc->thread_count == 0) {
        proc->exit_status = exit_status;
        // TODO: signal parent proc of death
    }

    while (true) {
        asm volatile ("hlt");
    }
    __builtin_unreachable();
}

struct syscall_ret sys_top() {
    printf("Pretend this is top()\n");

    struct syscall_ret ret = { 0, 0 };
    return ret;
}

struct syscall_ret sys_fork(struct interrupt_frame *r) {
    struct process *proc = vec_get(&process_list, running_thread->pid);
    if (proc->is_kernel) {
        panic("Cannot fork() the kernel\n");
    }

    struct process new_proc;
    struct thread *new_th = malloc(sizeof(struct thread));

    new_proc.pid = -1;
    new_proc.is_kernel = false;
    new_proc.parent = proc->pid;
    new_proc.thread_count = 1;

    pid_t new_pid = vec_push(&process_list, &new_proc);
    struct process *pnew_proc = vec_get(&process_list, new_pid);
    pnew_proc->pid = new_pid;
    vec_init_copy(&pnew_proc->fds, &proc->fds); // copy files to child

    new_th->tid = top_pid_tid++;
    new_th->stack = malloc(STACK_SIZE);
    new_th->rbp = new_th->stack + STACK_SIZE - sizeof(struct interrupt_frame);
    new_th->rsp = new_th->rbp;
    new_th->rip = return_from_interrupt;
    new_th->pid = new_pid;

    struct interrupt_frame *frame = new_th->rsp;
    memcpy(frame, r, sizeof(struct interrupt_frame));
    frame->rax = 0;
    frame->rcx = 0;

    pnew_proc->vm_root = vmm_fork();
    new_th->state = THREAD_RUNNING;
    
    enqueue_thread(new_th);

    struct syscall_ret ret = { pnew_proc->pid, 0 };
    return ret;
}

struct syscall_ret sys_getpid() {
    struct process *proc = vec_get(&process_list, running_thread->pid);
    struct syscall_ret ret = { proc->pid, 0 };
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
    struct process *proc = vec_get(&process_list, running_thread->pid);
    if (proc->is_kernel) {
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

    if (!check_elf(elf)) {
        // Bad file, cannot proceed
        ret.error = ENOEXEC;
        return ret;
    }
    // TODO: ensure we have enough unbacked space / backed space ?
    load_elf(elf);

    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame->rip = (uintptr_t)elf->e_entry;
    frame->user_rsp = 0x7FFFFF000000;
    frame->rbp = 0;
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame->rflags = 0x200;

    char *argument_data = (void *)0x7FFFFF002000;
    char **user_argv = (void *)0x7FFFFF001000;

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

    frame->rdi = argc;
    frame->rsi = (uintptr_t)user_argv;

    return ret; // value goes nowhere since rip moved.
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
        struct process *proc = vec_get(&process_list, process);
        if (proc->thread_count) {
            asm volatile ("hlt");
            continue;
        }
        ret.value = proc->exit_status;
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

    if ((uintptr_t)status > 0x800000000000) {
        // not in user mode
        // TODO: more granular permissions / address checking
        ret.error = EPERM;
        return ret;
    }

    while (true) {
        struct process *proc = vec_get(&process_list, process);

        if (!proc) {
            ret.error = ECHILD;
            return ret;
        }

        // TODO: permissions checking.
        if (proc->thread_count) {
            if (options & WNOHANG)  return ret;
            asm volatile ("hlt");
            continue;
        }

        *status = proc->exit_status;

        ret.value = process;
        return ret;
    }
}

struct syscall_ret sys_strace(bool enable) {
    struct syscall_ret ret = { 0, 0 };
    running_thread->strace = enable;
    return ret;
}

