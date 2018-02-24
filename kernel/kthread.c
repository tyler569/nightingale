
#include <basic.h>
#include <cpu/interrupt.h>
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
static kthread_t kthread_zero = { 0, THREAD_RUNNING, {}, NULL, NULL };
static kthread_t *current_kthread = &kthread_zero;
static int top_id = 1;

void kthread_swap(interrupt_frame *frame, kthread_t *old_kthread, kthread_t *new_kthread) {
    if (!current_kthread->next) {
        // Fallback to never get stuck
        current_kthread->next = &kthread_zero;
    }

    if (current_kthread == current_kthread->next)
        return;

    // 
    // These need to be set in the new frame
    //
    // This should be done at kthread_create time, but I don't currently have a way to.
    // So, I hack it here.
    //
    usize ss, cs;
    ss = frame->ss;
    cs = frame->cs;

    memcpy(&current_kthread->frame, frame, sizeof(interrupt_frame));
    do {
        current_kthread = current_kthread->next;

    // TEMP! For now, any other state is treated as dead, and not scheduled.
    } while (current_kthread->state != THREAD_RUNNING);
    memcpy(frame, &current_kthread->frame, sizeof(interrupt_frame));

    frame->ss = ss;
    frame->cs = cs;
    frame->rflags |= 0x200; // If the interrupt flag is disabled, we lock up becasue no more timer.
}

void test_kernel_thread() {
    printf("This is a test thread with pid %i\n", current_kthread->id);
    kthread_top();
    kthread_exit();
}

pid_t kthread_create(function_t entrypoint) {
    if (!current_kthread->next)
        current_kthread->next = current_kthread;

    kthread_t new_kthread = {
        .next = current_kthread->next, // to maintain the ring
        .id = ++top_id, // TEMP HACK
        .state = THREAD_RUNNING,
        .parent = current_kthread,
        .frame = {
            .rip = (usize)entrypoint,
            .user_rsp = (usize)malloc(4096),
            .cs = 0, // SOMETHING
            .ss = 0, // SOMETHING - these are currently set above.
        },
    };

    current_kthread->next = malloc(sizeof(kthread_t));
    memcpy(current_kthread->next, &new_kthread, sizeof(kthread_t));

    return top_id; // Probable race condition in top_id being global like this.
}

void kthread_exit() {
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

