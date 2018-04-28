
#include <basic.h>
#include <arch/x86/cpu.h>
#include <arch/x86/interrupt.h>
#include <malloc.h>
#include <panic.h>
#include <string.h>
#include <vmm.h>
#include <syscall.h> // for sys_exit
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
    /*
    printf("SWAPPING %i -> %i\n", 
            current_kthread->id, current_kthread->next->id);

    printf("new rip: %#lx, new rsp: %#lx\n",
            current_kthread->next->frame.rip, current_kthread->next->frame.user_rsp);
    */

    memcpy(&current_kthread->frame, frame, sizeof(interrupt_frame));
    // debug_print_kthread(current_kthread);
    do {
        current_kthread = current_kthread->next;
    } while (current_kthread->state != THREAD_RUNNING); // TEMP handle states
    memcpy(frame, &current_kthread->frame, sizeof(interrupt_frame));
    // debug_print_kthread(current_kthread);

    // Trying this in create
    // frame->rflags |= 0x200; // interrupt flag, so we don't lock
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
    asm volatile ("hlt");
    __builtin_unreachable();
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
