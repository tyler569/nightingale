
#include <basic.h>
#include <cpu/interrupt.h>
#include <malloc.h>
#include <panic.h>
#include <string.h>
#include "proc.h"

// 
// For the moment, processes are stored in memory as a ring, and given
// round-robin access to the processor.
//
// Each Proc contains a ->next which is the next process in the ring.
//
// They also store their parent (the Proc that launched them) for
// later use.
//
// This will almost certainly all be changed later.
//
static Proc proc_zero = { 0, PROC_RUNNING, {}, NULL, NULL };
static Proc *current_proc = &proc_zero;
static int top_id = 1;

void do_process_swap(interrupt_frame *frame) {
    if (!current_proc->next) {
        current_proc->next = &proc_zero;
        // Could this abandon processes?
    }

    // 
    // These need to be set in the new frame
    //
    // This should be done at proc_create time, but I don't currently have a way to.
    // So, I hack it here.
    //
    usize ss, cs;
    ss = frame->ss;
    cs = frame->cs;

    memcpy(&current_proc->frame, frame, sizeof(interrupt_frame));
    do {
        current_proc = current_proc->next;

    // TEMP! For now, any other state is treated as dead, and not scheduled.
    } while (current_proc->state != PROC_RUNNING);
    memcpy(frame, &current_proc->frame, sizeof(interrupt_frame));

    frame->ss = ss;
    frame->cs = cs;
    frame->rflags |= 0x200; // If the interrupt flag is disabled, we lock up becasue no more timer.
}

void test_kernel_thread() {
    printf("This is a test thread with pid %i\n", current_proc->id);
    proc_top();
    proc_exit();
}

Pid proc_create(Entrypoint entrypoint) {
    usize new_proc_stack = (usize)malloc(4096);

    Proc new_proc = {
        .next = current_proc->next, // to maintain the ring
        .id = ++top_id, // TEMP HACK
        .state = PROC_RUNNING,
        .parent = current_proc,
        .frame = {
            .rip = (usize)entrypoint,
            .user_rsp = (usize)malloc(4096),
            .cs = 0, // SOMETHING
            .ss = 0, // SOMETHING - these are currently set above.
        },
    };

    Proc *old_next = current_proc->next;
    current_proc->next = malloc(sizeof(Proc));
    memcpy(current_proc->next, &new_proc, sizeof(Proc));

    return top_id; // Probable race condition in top_id being global like this.
}

void proc_exit() {
    assert(current_proc->id != 0, "Cannot kill pid 0 (well, I guess you did...)");

    current_proc->state = PROC_KILLED;
    asm volatile ("hlt");
    // proc will be cleaned up later (TODO)
}

void proc_top() {
    Proc *current = current_proc;

    printf("Proc %i is currently running\n", current_proc->id);
    printf("Ring: ");
    do {
        printf("%i -> ", current->id);
        current = current->next;
    } while (current->id != current_proc->id);
    printf("%i\n", current->id);
}

