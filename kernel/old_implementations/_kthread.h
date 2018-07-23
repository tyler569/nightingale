
#ifndef NIGHTINGALE_KTHREAD_H
#define NIGHTINGALE_KTHREAD_H

#include <basic.h>
#include <arch/x86/interrupt.h>

typedef u32 pid_t;

enum thread_state {
    THREAD_INVALID = 0,
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_KILLED,
};

struct kthread {
    pid_t id;
    enum thread_state state;

    void *stack; // saved for freeing

    struct kthread *next;
    struct kthread *prev;
    struct kthread *parent;

    interrupt_frame frame;
    uintptr_t vm_root; // PML4 root of this thread's VM tree (physical addr)

    char stack_content[0x1000];

    bool strace;
};

extern struct kthread *current_kthread;
extern uintptr_t boot_pml4;

void test_kernel_thread();

void swap_kthread(interrupt_frame *frame, struct kthread *old_kthread, struct kthread *new_kthread);
pid_t create_kthread(void *entrypoint);
pid_t create_user_thread(void *entrypoint);
void exit_kthread();

int count_running_threads();
void kthread_top();
void debug_print_kthread(struct kthread *thread);

void thread_watchdog();

#endif

