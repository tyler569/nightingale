
#include <basic.h>
#include <arch/x86/cpu.h>
#include <arch/x86/interrupt.h>
#include <malloc.h>
#include <panic.h>
#include <string.h>
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
    .next = NULL,
    .parent = NULL,
    .frame = {}
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

    // printf("SWAPPING %i -> %i\n", 
    //        current_kthread->id, current_kthread->next->id);

    // 
    // These need to be set in the new frame
    //
    // This should be done at kthread_create time,
    // but I don't currently have a way to.
    // So, I hack it here.
    //
    // This is also where user-mode is actually distinguished
    // so, I can't keep it like this for long!
    //
    usize ss, cs;
    ss = frame->ss;
    cs = frame->cs;

    memcpy(&current_kthread->frame, frame, sizeof(interrupt_frame));
    // debug_print_kthread(current_kthread);
    do {
        current_kthread = current_kthread->next;
    } while (current_kthread->state != THREAD_RUNNING); // TEMP handle states
    memcpy(frame, &current_kthread->frame, sizeof(interrupt_frame));
    // debug_print_kthread(current_kthread);

    // see above
    frame->ss = ss;
    frame->cs = cs;
    frame->rflags |= 0x200; // If the interrupt flag is disabled, we lock up becasue no more timer.
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
    kthread_t new_kthread = {
        .next = current_kthread->next, // to maintain the ring
        .id = new_id,
        .state = THREAD_RUNNING,
        .parent = current_kthread,
        .frame = {
            .rip = (uintptr_t)entrypoint,
            .user_rsp = ((uintptr_t)malloc(stack_size)) + stack_size,
            .cs = 0, // SOMETHING
            .ss = 0, // SOMETHING - these are currently set above.
        },
    };

    kthread_t *new_th = malloc(sizeof(kthread_t));
    memcpy(new_th, &new_kthread, sizeof(kthread_t));

    current_kthread->next = new_th;

    return new_id;
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
