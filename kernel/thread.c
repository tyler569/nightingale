
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include "thread.h"

extern uintptr_t boot_pml4;

struct thread_queue *runnable_threads = NULL;
struct thread_queue *runnable_threads_tail = NULL;

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
        if (!runnable_threads) {
            to = &thread_zero;
        } else {
            to = runnable_threads->sched;
            runnable_threads = runnable_threads->next;
            enqueue_thread(to); // <- shitty way to do this
        }
    }
   
    set_kernel_stack(to->stack);
    set_vm_root(to->proc->vm_root);

    asm volatile ("mov %%rsp, %0" : "=r"(running_thread->rsp));
    asm volatile ("mov %%rbp, %0" : "=r"(running_thread->rbp));
    uintptr_t rip = read_rip();

    if (rip == 0x99) {
        // task switch completed and we have returned to this one
        return;
    }

    to->rip = (void *)rip;

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

void sys_fork() {}
void sys_top() {}
void sys_exit() {}

