
#include <basic.h>
#include <arch/x86/cpu.h>
#include <arch/x86/interrupt.h>
#include <malloc.h>
#include <panic.h>
#include <string.h>
#include <vmm.h>
#include <arch/x86/cpu.h>
#include <syscall.h>
#include <syscalls.h>
#include "kthread.h"


// 
// For the moment, threads are stored in memory as a ring, and given
// round-robin access to the processor.
//
// Each kthread_t contains a ->next which is the next thread in the ring.
//
// They also store their parent (the kthread_t that launched them) for
// later use.
//
// This will almost certainly all be changed later.
//
static kthread_t kthread_zero = { 
    .id = 0,
    .state = THREAD_RUNNING,
    .stack = NULL,
    .next = NULL,
    .prev = NULL,
    .parent = NULL,
    .frame = {},
    .strace = 0,
    .vm_root = (uintptr_t)&boot_pml4,
};

kthread_t *current_kthread = &kthread_zero;
static int top_id = 0;

void swap_kthread(interrupt_frame *frame, kthread_t *old_kthread, kthread_t *new_kthread) {
    if (!current_kthread->next) {
        // Fallback to never get stuck
        current_kthread->next = &kthread_zero;
    }

    if (current_kthread == current_kthread->next)
        return;

    uintptr_t current_vm = current_kthread->vm_root;

    memcpy(&current_kthread->frame, frame, sizeof(interrupt_frame));

    do {
        current_kthread = current_kthread->next;
    } while (current_kthread->state != THREAD_RUNNING); // TEMP handle states

    if (current_vm != current_kthread->vm_root) {
        // if we need to swap the VM, then do
        // everything here should be in kernel space and be the same in all VMs
        // in entry 511 (for x86_64 of course)
        //
        // This would need to change for different arches as well
        // TODO "swap_vm_table" function maybe in arch/ ?
        asm volatile ("mov %0, %%cr3" :: "r"(current_kthread->vm_root));
    }

    memcpy(frame, &current_kthread->frame, sizeof(interrupt_frame));
}

void test_kernel_thread() {
    printf("This is a test thread with pid %i\n", current_kthread->id);
    kthread_top();
    exit_kthread();
}

pid_t create_kthread(function_t entrypoint) {
    if (!current_kthread->next)
        current_kthread->next = current_kthread;

    pid_t new_id = ++top_id; // TODO: be intelligent about this

    size_t stack_size = 0x1000;
    void *stack = malloc(stack_size);
    if (stack == NULL) {
        panic("Error creating thread with pid %i, OOM (NULL from malloc)\n", new_id);
    }

    kthread_t new_kthread = {
        .next = current_kthread->next, // to maintain the ring
        .id = new_id,
        .state = THREAD_RUNNING,
        .stack = stack,
        .parent = current_kthread,
        .prev = current_kthread,
        .frame = {
            .rip = (uintptr_t)entrypoint,
            .user_rsp = (uintptr_t)stack + stack_size,
            .cs = 8,
            .ss = 0, // anything?
            .rflags = 0x200, // interrupt flag, so we don't lock
        },
        .vm_root = (uintptr_t)&boot_pml4,
    };

    kthread_t *new_th = malloc(sizeof(kthread_t));
    if (new_th == NULL) {
        panic("Error creating thread with pid %i, OOM (NULL from malloc)\n", new_id);
    }
    memcpy(new_th, &new_kthread, sizeof(kthread_t));

    current_kthread->next = new_th;

    printf("created thread %i\n", new_th->id);

    return new_id;
}

// COPYPASTA from above
//
pid_t create_user_thread(function_t entrypoint) {
    if (!current_kthread->next)
        current_kthread->next = current_kthread;

    pid_t new_id = ++top_id; // TODO: be intelligent about this

    size_t stack_size = 0x1000;

    void *stack = (void *)0x7FFFFF000000;
    vmm_create_unbacked((uintptr_t)stack, PAGE_USERMODE | PAGE_WRITEABLE);

    kthread_t new_kthread = {
        .next = current_kthread->next, // to maintain the ring
        .id = new_id,
        .state = THREAD_RUNNING,
        .stack = stack,
        .parent = current_kthread,
        .prev = current_kthread,
        .frame = {
            .rip = (uintptr_t)entrypoint,
            .user_rsp = (uintptr_t)stack + stack_size,
            .cs = 0x10 | 3,
            .ss = 0x18 | 3,
            .rflags = 0x200, // interrupt flag, so we don't lock
        },
        .strace = false,
        .vm_root = (uintptr_t)&boot_pml4,
    };

    kthread_t *new_th = malloc(sizeof(kthread_t));
    if (new_th == NULL) {
        panic("Error creating thread with pid %i, OOM (NULL from malloc)\n", new_id);
    }
    memcpy(new_th, &new_kthread, sizeof(kthread_t));

    current_kthread->next = new_th;

    return new_id;
}

struct syscall_ret sys_exit(int exit_status) {
    current_kthread->state = THREAD_KILLED;
    // send signal with exit status to parent
    while (true) {
        asm volatile ("hlt");
    }
    __builtin_unreachable();
}

struct syscall_ret sys_fork(interrupt_frame *frame) {
    kthread_t *tmp = current_kthread->next;
    kthread_t *fork_th = malloc(sizeof(kthread_t));

    pid_t child_id = ++top_id;

    uintptr_t new_vm = vmm_fork();

    memcpy(&current_kthread->frame, frame, sizeof(interrupt_frame)); // save current context
    memcpy(fork_th, current_kthread, sizeof(*current_kthread));      // copy new thread

    fork_th->id = child_id;
    fork_th->parent = current_kthread;
    fork_th->vm_root = new_vm;
    
    current_kthread->next = fork_th;
    fork_th->next = tmp;

    fork_th->frame.rax = 0; // forked thread's return value
    fork_th->frame.rcx = 0; // forked thread's error value
    printf("new rip: %lx\n", fork_th->frame.rip);

    struct syscall_ret ret = {};
    ret.value = child_id;
    ret.error = 0;
    return ret;
}

struct syscall_ret sys_top(void) {
    kthread_top();
    struct syscall_ret ret = {0};
    return ret;
}

void thread_watchdog() {
    kthread_t *cur = current_kthread;
    kthread_t *tmp = cur->next;

    while (true) {

        if (!cur->next)
            panic("No next thread!");

        tmp = cur->next;

        if (tmp == current_kthread) {
            asm volatile ("hlt");
            continue;
        }
        if (tmp->state == THREAD_RUNNING) {
            cur = cur->next;
            continue;
        } else {
            // if it's not running, kill it!

            printf("killing pid %i\n", tmp->id);

            cur->next = tmp->next;
            if (tmp->stack > (void *)0xFFFF000000000000) {
                free(tmp->stack);
            }
            free(tmp);
        }
    }
}

int count_running_threads() {
    int count = 0;

    for (kthread_t *c = current_kthread; ; c = c->next) {
        if (c->state == THREAD_RUNNING)
            count++;
        if (c->next == current_kthread)
            break;
    }

    return count;
}

void exit_kthread() {
    assert(current_kthread->id != 0, "Cannot kill pid 0 (well, I guess you did...)");

    current_kthread->state = THREAD_KILLED;

    asm volatile ("hlt");
    // kthread will be cleaned up later (TODO)
}

void kthread_top() {
    kthread_t *current = current_kthread;

    printf("Thread %i is currently running\n", current_kthread->id);
    printf("Running thread ring: ");
    do {
        printf("%i -> ", current->id);
        current = current->next;
    } while (current->id != current_kthread->id);
    printf("%i\n", current->id);
}

void debug_print_kthread(kthread_t *thread) {
    printf("thread %i {\n", thread->id);
    printf("  state: %s\n", thread->state == THREAD_RUNNING ? "alive" : "dead");
    printf("  frame:\n");
    print_registers(&thread->frame);
    if ((uintptr_t)thread->next > 0x1000) {
        printf("  next: %#x (pid %i)\n", thread->next, thread->next->id);
    } else {
        printf("  next: %#x (NULL)\n", thread->next);
    }

    if ((uintptr_t)thread->parent > 0x1000) {
        printf("  parent: %#x (pid %i)\n", thread->parent, thread->parent->id);
    } else {
        printf("  parent: %#x (NULL)\n", thread->parent);
    }
    printf("}\n");

    printf("THIS THREAD IN MEM:\n");
    debug_dump_after(thread);
    printf("THIS->NEXT THREAD IN MEM:\n");
    debug_dump_after(thread->next);
}
