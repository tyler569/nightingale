
#pragma once
#ifndef NIGHTINGALE_THREAD_H
#define NIGHTINGALE_THREAD_H

#include <stdint.h>
#include <stddef.h>
#include <ds/queue.h>
#include <ds/vector.h>

typedef int pid_t;

typedef struct fp_ctx {
    // on x86, the floating point context for a process is an opaque
    // 512 byte region.  This is probably not suuuper portable;
    char data[512];
} fp_ctx;

struct process {
    pid_t pid;
    uintptr_t vm_root;

    int uid;
    int gid;

    int thread_count;
    int exit_status;

    pid_t parent;

    struct vector fds;
    struct queue blocked_threads;

    int refcnt;
};

enum thread_state {
    THREAD_RUNNING = 1,
    THREAD_DONE,
    THREAD_KILLED_FOR_VIOLATION,
    THREAD_BLOCKED,
};

struct thread {
    fp_ctx fpctx;

    pid_t tid;
    pid_t pid;
    bool running;
    bool strace;

    int state;
    int exit_status;

    char* stack;

    void* sp;
    void* bp;
    uintptr_t ip;
};

struct thread* running_thread;
struct process* running_process;

enum {
    SW_TIMEOUT,
    SW_BLOCK,
    SW_YIELD,
};

void threads_init(void);
void switch_thread(int reason);
void new_kernel_thread(uintptr_t entrypoint);
void new_user_process(uintptr_t entrypoint);
void kill_running_thread(int exit_code);
void block_thread(struct queue* threads);
void wake_blocked_threads(struct queue* threads);

#endif

