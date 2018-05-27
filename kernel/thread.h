
#pragma once
#ifndef NIGHTINGALE_THREAD_H
#define NIGHTINGALE_THREAD_H

#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
// #include "thread.h"

typedef int pid_t;

struct process {
    pid_t pid;

    bool is_kernel;

    uintptr_t vm_root;

    struct process *parent;
};

enum thread_state {
    THREAD_RUNNING = 1,
    THREAD_KILLED,
};

struct thread {
    pid_t tid;

    bool running;
    bool strace;

    int state;

    void *stack;

    void *rsp;
    void *rbp;
    void *rip;

    struct process *proc;
};

struct thread_queue {
    struct thread *sched;
    struct thread_queue *next;
};

extern struct thread_queue *runnable_threads;
extern struct thread_queue *runnable_threads_tail;

#define STACK_SIZE 0x1000

extern struct thread *running_thread;

void switch_thread(struct thread *to);
void new_kernel_thread(void *entrypoint);

#endif

