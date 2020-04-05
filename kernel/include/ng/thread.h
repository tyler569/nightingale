
#pragma once
#ifndef NG_THREAD_H
#define NG_THREAD_H

#include <stddef.h>
#include <stdint.h>
#include <ng/fs.h>
#include <ng/list.h>
#include <ng/dmgr.h>
#include <ng/signal.h>

typedef struct fp_ctx {
        // on x86, the floating point context for a process is an opaque
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

        int exit_status;
        
        struct process *parent;

        struct dmgr fds;
        list children;
        list threads;

        list_n siblings;

        struct file *procfile;

        uintptr_t mmap_base;

        sighandler_t sigactions[16];
        uint32_t signal_pending;
};

enum thread_state {
        THREAD_INVALID = 0,
        THREAD_RUNNING,
        THREAD_BLOCKED,
        THREAD_DONE,
        THREAD_KILLED,
        THREAD_SLEEP,
};

enum thread_flags {
        THREAD_STRACE    = 0x0001,
        THREAD_WAIT      = 0x0002,
        THREAD_IN_SIGNAL = 0x0004,
        THREAD_AWOKEN    = 0x0008,
        THREAD_QUEUED    = 0x0010,
        THREAD_ONCPU     = 0x0020,
};

struct thread {
        pid_t tid;
        struct process *proc;

        enum thread_state thread_state;
        enum thread_flags thread_flags;

        char *stack;

        void *sp;
        void *bp;
        uintptr_t ip;

        struct file *cwd;

        pid_t wait_request;
        struct process *wait_result;

        list_n runnable;
        list_n freeable;
        list_n process_threads;
        list_n blocking_node;

        struct timer_event *blocking_event;

        uintptr_t user_sp;
        struct signal_context signal_context;

        fp_ctx fpctx;
};

extern struct thread *running_thread;
extern struct process *running_process;

enum switch_reason {
        SW_TIMEOUT,
        SW_BLOCK,
        SW_YIELD,
        SW_DONE,
        SW_REQUEUE,
};

void return_from_interrupt(void);
void set_kernel_stack(void *);

void threads_init(void);
void switch_thread(enum switch_reason reason);
pid_t new_kthread(uintptr_t entrypoint);
noreturn void exit_kthread(void);
pid_t new_user_process(uintptr_t entrypoint);
void block_thread(struct list *threads);
void wake_blocked_threads(struct list *threads);
void kill_process_group(pid_t pgid);
void kill_process(struct process *p);
void kill_pid(pid_t pid);
struct process *process_by_id(pid_t pid);
pid_t bootstrap_usermode(const char *init_filename);
void wake_process_thread(struct process *p);
void drop_thread(struct thread *);
void enqueue_thread(struct thread *);
void enqueue_thread_at_front(struct thread *);

#endif // NG_THREAD_H

