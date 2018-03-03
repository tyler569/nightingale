
#ifndef NIGHTINGALE_KTHREAD_H
#define NIGHTINGALE_KTHREAD_H

#include <basic.h>
#include <arch/x86/interrupt.h>

typedef u32 pid_t;

typedef enum thread_state {
    THREAD_RUNNING,
    THREAD_WAITING,
    THREAD_SLEEPING,
    THREAD_KILLED,
    THREAD_DEAD
} thread_state_t;

typedef struct kthread {
    pid_t id;
    thread_state_t state;

    interrupt_frame frame;
    struct kthread *next;
    struct kthread *parent;
} kthread_t;

typedef void function_t();

void test_kernel_thread();

void kthread_swap(interrupt_frame *frame, kthread_t *old_kthread, kthread_t *new_kthread);
pid_t kthread_create(function_t entrypoint);
void kthread_exit();

void kthread_top();

#endif

