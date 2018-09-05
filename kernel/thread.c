
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include <string.h>
#include <panic.h>
#include <debug.h>
#include <vector.h>
#include <arch/x86/cpu.h>
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
    // .proc = &proc_zero,
    .pid = 0,
};

struct thread *running_thread = &thread_zero;

void init_threads() {
    vec_init(&process_list, struct process);
    printf("threads: thread data at %p\n", process_list.data);
    vec_push(&process_list, &proc_zero);
    // do for threads too?
}

void set_kernel_stack(void *stack_top) {
    extern uintptr_t *kernel_stack;
    *&kernel_stack = stack_top;
}

void set_vm_root(uintptr_t vm_root) {
    asm volatile ("mov %0, %%cr3" :: "r"(vm_root));
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

// currently in boot.asm
uintptr_t read_rip();

void switch_thread(struct thread *to) {
    if (to == NULL) {
        do {
            if (!runnable_threads) {
                to = &thread_zero;
                break;
            }
            to = runnable_threads->sched;
            struct thread_queue *old = runnable_threads;
            runnable_threads = runnable_threads->next;
            free(old);
        } while (!(to->state == THREAD_RUNNING));

        if (!(to == &thread_zero)) {
            enqueue_thread(to); // <- shitty way to do this
        }
    }
    struct process *to_proc = vec_get(&process_list, to->pid);
    // printf("swapping %i -> %i\n", running_thread->tid, to->tid);
    // printf("going to rip:%#lx\n", to->rip);
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

    running_thread = to;

    asm volatile (
            "mov %0, %%rbx\n\t"
            "mov %1, %%rsp\n\t"
            "mov %2, %%rbp\n\t"
            "mov $0x99, %%rax\n\t" /* This makes read_rip return 0x99 when we switch back to it */
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
    // notify main thread of death?
    assert(proc->thread_count >= 0, "killed more threads than exist...");

    if (proc->thread_count == 0) {
        proc->exit_status = exit_status;
        // notify parent proc of death?
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
    // struct process *proc = malloc(sizeof(struct process));
    struct process proc;
    struct thread *th = malloc(sizeof(struct thread));

    memset(th, 0, sizeof(struct thread));

    proc.pid = -1; // top_pid_tid++;
    proc.is_kernel = false;
    proc.parent = 0;
    proc.thread_count = 1;

    pid_t pid = vec_push(&process_list, &proc);
    struct process *pproc = vec_get(&process_list, pid);
    pproc->pid = pid;

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
    // DON'T ALLOCATE 0x0000 FOR STACK UNDERFLOW PROTECTION
    vmm_create_unbacked_range(0x7FFFFF000000 - 0x10000, 0x10000, PAGE_USERMODE | PAGE_WRITEABLE);
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame->rflags = 0x200;

    pproc->vm_root = vmm_fork();
    th->state = THREAD_RUNNING;

    enqueue_thread(th);
}


struct syscall_ret sys_exit(int exit_status) {
    running_thread->state = THREAD_DONE; // LEAK
    running_thread->exit_status = exit_status;

    struct process *proc = vec_get(&process_list, running_thread->pid);
    proc->thread_count -= 1;
    // notify main thread of death?
    assert(proc->thread_count >= 0, "killed more threads than exist...");

    if (proc->thread_count == 0) {
        proc->exit_status = exit_status;
        // notify parent proc of death?
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

    // struct process *new_proc = malloc(sizeof(struct process));
    struct process new_proc;
    struct thread *new_th = malloc(sizeof(struct thread));

    new_proc.pid = -1;
    new_proc.is_kernel = false;
    new_proc.parent = proc->pid;
    new_proc.thread_count = 1;

    pid_t new_pid = vec_push(&process_list, &new_proc);
    struct process *pnew_proc = vec_get(&process_list, new_pid);
    pnew_proc->pid = new_pid;

    new_th->tid = top_pid_tid++;
    new_th->stack = malloc(STACK_SIZE);
    new_th->rbp = new_th->stack + STACK_SIZE - sizeof(struct interrupt_frame);
    new_th->rsp = new_th->rbp;
    new_th->rip = return_from_interrupt;
    new_th->pid = new_pid;
    // new_th->strace = true;

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

    // printf("file at %lx\n", file);

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

    load_elf(elf);

    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame->rip = (uintptr_t)elf->e_entry;
    frame->user_rsp = 0x7FFFFF000000;
    frame->rbp = 0x7FFFFF000000;
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame->rflags = 0x200;

    // DON'T ALLOCATE 0x0000 FOR STACK UNDERFLOW PROTECTION
    vmm_create_unbacked_range(0x7FFFFF000000 - 0x10000, 0x10000, PAGE_USERMODE | PAGE_WRITEABLE);
    vmm_create_unbacked_range(0x7FFFFF001000, 0x2000, PAGE_USERMODE | PAGE_WRITEABLE);


    char *argument_data = (void *)0x7FFFFF002000;
    char **user_argv = (void *)0x7FFFFF001000;

    size_t argc = 0;
    // printf("argv = %#lp\n", argv);
    user_argv[argc++] = argument_data;
    argument_data = strcpy(argument_data, filename);
    argument_data += 1;
    while (*argv) {
        // printf("processing argument: %s\n", *argv);
        user_argv[argc++] = argument_data;
        argument_data = strcpy(argument_data, *argv);
        argument_data += 1;
        argv += 1;
    }

    frame->rdi = argc;
    frame->rsi = (uintptr_t)user_argv;

    // It looks like I have to fix the page mapping when I do this
    // not sure why - this might be a bandaid??!
    // TODO: investigate and fix this
    invlpg(0x400000);

    // debug:
    // printf("entry: %#lx\n", elf->e_entry);
    // dump_mem((void *)0x400000, 0x100);

    return ret; // goes nowhere since rip moved.
}

struct syscall_ret sys_wait4(pid_t process) {
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

