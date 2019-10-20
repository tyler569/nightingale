
#pragma once
#ifndef NIGHTINGALE_THREAD_H
#define NIGHTINGALE_THREAD_H

#include <stddef.h>
#include <stdint.h>
#include <ng/fs.h>
#include <ds/list.h>
#include <ds/dmgr.h>

typedef struct fp_ctx { // on x86, the floating point context for a process is an opaque
        // 512 byte region.  This is probably not suuuper portable;
        char data[512];
} _align(16) fp_ctx;

struct process {
        pid_t pid;
        pid_t pgid;
        char *comm;

        uintptr_t vm_root;

        int uid;
        int gid;

        int status;
        
        struct process *parent;

        struct dmgr fds;
        struct list children;
        struct list threads;

        uintptr_t mmap_base;
};

enum thread_state {
        THREAD_RUNNING = 0,
        THREAD_BLOCKED = -1,
        THREAD_KILLED_FOR_VIOLATION = -2,
        THREAD_DONE = 1,
};

#define THREAD_STRACE 0x0001
#define THREAD_WAIT   0x0002

struct thread {
        pid_t tid;
        struct process *proc;

        int state;
        int flags;

        char *stack;

        void *sp;
        void *bp;
        uintptr_t ip;

        struct fs_node *cwd;

        int request_status; // request waitpid update from <process>
        struct process *status_resp;

        fp_ctx fpctx;
};

extern struct thread *running_thread;
extern struct process *running_process;

enum {
        SW_TIMEOUT,
        SW_BLOCK,
        SW_YIELD,
};

void threads_init(void);
void switch_thread(int reason);
void new_kthread(uintptr_t entrypoint);
noreturn void exit_kthread(void);
void new_user_process(uintptr_t entrypoint);
void kill_running_thread(int exit_code);
void block_thread(struct list *threads);
void wake_blocked_threads(struct list *threads);

struct process *process_by_id(pid_t pid);
void bootstrap_usermode(const char *init_filename);

#endif
