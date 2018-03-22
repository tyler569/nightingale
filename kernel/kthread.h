
#ifndef NIGHTINGALE_KTHREAD_H
#define NIGHTINGALE_KTHREAD_H

#include <basic.h>
#include <arch/x86/interrupt.h>

typedef u32 pid_t;

typedef enum thread_state {
    THREAD_INVALID = 0,
    THREAD_RUNNING,
    THREAD_WAITING,
    THREAD_SLEEPING,
    THREAD_KILLED,
    THREAD_DEAD
} thread_state_t;

typedef struct kthread {
    pid_t id;
    thread_state_t state;

    void *stack; // saved for freeing

    struct kthread *next;
    struct kthread *prev;
    struct kthread *parent;

    interrupt_frame frame;
} kthread_t;

extern struct kthread *current_kthread;

typedef void function_t();

void test_kernel_thread();

void swap_kthread(interrupt_frame *frame, kthread_t *old_kthread, kthread_t *new_kthread);
pid_t create_kthread(function_t entrypoint);
pid_t create_user_thread(function_t entrypoint);
void exit_kthread();

int count_running_threads();
void kthread_top();
void debug_print_kthread(kthread_t *thread);

void thread_watchdog();

#endif

