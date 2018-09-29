
#pragma once
#ifndef NIGHTINGALE_THREAD_H
#define NIGHTINGALE_THREAD_H

#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <queue.h>
#include "vector.h"

typedef int pid_t;

struct process {
    pid_t pid;
    bool is_kernel;
    uintptr_t vm_root;

    int uid;
    int gid;

    int thread_count;
    int exit_status;

    pid_t parent;

    struct vector fds;
    struct queue blocked_threads;
};

enum thread_state {
    THREAD_RUNNING = 1,
    THREAD_DONE,
    THREAD_KILLED_FOR_VIOLATION,
    THREAD_BLOCKED,
};

struct thread {
    pid_t tid;

    bool running;
    bool strace;

    int state;
    int exit_status;

    char* stack;

    void* sp;
    void* bp;
    uintptr_t ip;

    pid_t pid;
};

/*
struct thread_queue {
    struct thread* sched;
    struct thread_queue* next;
};

extern struct thread_queue* runnable_threads;
extern struct thread_queue* runnable_threads_tail;

^^ did I even use those anywhere?
*/

extern struct thread* running_thread;
extern struct process* running_process;

void threads_init(void);
void switch_thread(struct thread* to);
void new_kernel_thread(uintptr_t entrypoint);
void new_user_process(uintptr_t entrypoint);
void kill_running_thread(int exit_code);
void block_thread(struct queue* threads);
void wake_blocked_threads(struct queue* threads);

#endif

