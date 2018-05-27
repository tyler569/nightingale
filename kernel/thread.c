
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include <string.h>
#include <panic.h>
#include <arch/x86/cpu.h>
#include <vmm.h>
#include "thread.h"

extern uintptr_t boot_pml4;

struct thread_queue *runnable_threads = NULL;
struct thread_queue *runnable_threads_tail = NULL;

int top_pid_tid = 1;

struct process proc_zero = {
    .pid = 0,
    .is_kernel = true,
    .vm_root = (uintptr_t)&boot_pml4,
    .parent = NULL,
};

extern char boot_kernel_stack;

struct thread thread_zero = {
    .tid = 0,
    .running = true,
    .strace = false,
    .stack = &boot_kernel_stack,
    .state = THREAD_RUNNING,
    .proc = &proc_zero,
};

struct thread *running_thread = &thread_zero;

void threads_init() {
    
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
            runnable_threads = runnable_threads->next;
            // This drops dead threads on the floor for now.
        } while (!(to->state == THREAD_RUNNING));

        if (!(to == &thread_zero)) {
            enqueue_thread(to); // <- shitty way to do this
        }
    }

    // printf("swapping %i -> %i\n", running_thread->tid, to->tid);
    // printf("going to rip:%#lx\n", to->rip);
  
    set_kernel_stack(to->stack + STACK_SIZE);
    set_vm_root(to->proc->vm_root);

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

void new_kernel_thread(void *entrypoint) {
    struct thread *th = malloc(sizeof(struct thread));
    memset(th, 0, sizeof(struct thread));

    th->proc = &proc_zero;
    th->stack = malloc(STACK_SIZE);

    th->rip = entrypoint;
    th->rsp = th->stack + STACK_SIZE;
    th->rbp = th->rsp;

    th->tid = top_pid_tid++;

    th->state = THREAD_RUNNING;

    enqueue_thread(th);
}

void return_from_interrupt();

void new_user_process(void *entrypoint) {
    struct process *proc = malloc(sizeof(struct process));
    struct thread *th = malloc(sizeof(struct thread));

    memset(th, 0, sizeof(struct thread));

    proc->pid = top_pid_tid++;
    proc->is_kernel = false;
    proc->parent = NULL;

    th->tid = top_pid_tid++;
    th->stack = malloc(STACK_SIZE);
    th->rbp = th->stack + STACK_SIZE - sizeof(struct interrupt_frame);
    th->rsp = th->rbp;
    th->rip = return_from_interrupt;
    th->proc = proc;

    struct interrupt_frame *frame = th->rsp;
    memset(frame, 0, sizeof(struct interrupt_frame));
    frame->ds = 0x18 | 3;
    frame->rip = (uintptr_t)entrypoint;
    frame->user_rsp = 0x7FFFFF000000 + 0x1000;
    vmm_create_unbacked(0x7FFFFF000000, PAGE_USERMODE | PAGE_WRITEABLE);
    frame->cs = 0x10 | 3;
    frame->ss = 0x18 | 3;
    frame->rflags = 0x200;

    proc->vm_root = vmm_fork();
    th->state = THREAD_RUNNING;

    enqueue_thread(th);
}


void sys_exit() {
    if (running_thread->proc->is_kernel) {
        running_thread->state = THREAD_KILLED;
    }
    asm volatile ("hlt");
}
void sys_top() {}
void sys_fork() {}
