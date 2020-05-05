
#pragma once
#ifndef NG_THREAD_H
#define NG_THREAD_H

#include <stddef.h>
#include <stdint.h>
#include <ng/dmgr.h>
#include <ng/fs.h>
#include <ng/signal.h>
#include <ng/trace.h>
#include <nc/list.h>
#include <nc/signal.h>

#define PROC_MAGIC    0x434f5250 // 'PROC'
#define THREAD_MAGIC  0x44524854 // 'THRD'

#define COMM_SIZE 32

typedef struct fp_ctx {
        // on x86, the floating point context for a process is an opaque
        // 512 byte region.  This is probably not suuuper portable;
        char data[512];
} _align(16) fp_ctx;

struct process {
        pid_t pid;
        pid_t pgid;
        char comm[COMM_SIZE];

        unsigned int magic; // PROC_MAGIC

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
        THREAD_STRACE     = (1 << 0),
        THREAD_WAIT       = (1 << 1),
        THREAD_IN_SIGNAL  = (1 << 2),
        THREAD_AWOKEN     = (1 << 3),
        THREAD_QUEUED     = (1 << 4),
        THREAD_ONCPU      = (1 << 5),
        THREAD_IS_KTHREAD = (1 << 6),
};

struct thread {
        pid_t tid;
        struct process *proc;

        unsigned int magic; // THREAD_MAGIC

        enum thread_state state;
        enum thread_flags flags;

        char *kstack;

        void *sp;
        void *bp;
        uintptr_t ip;

        struct file *cwd;

        pid_t wait_request;
        struct thread *wait_result;

        struct thread *tracer;
        enum trace_state trace_state;
        interrupt_frame *trace_frame;

        list_n runnable;
        list_n freeable;
        list_n process_threads;
        list_n wait_node;

        struct file *procfile;

        struct timer_event *wait_event;

        uintptr_t user_sp;
        struct signal_context signal_context;

        sighandler_t sighandlers[32];
        sigset_t sig_pending;
        sigset_t sig_mask;

        long n_scheduled;
        long time_ran;

        /* if ONCPU, time placed ONCPU.
         * else time last pulled off */
        long last_scheduled;

        fp_ctx fpctx;
};

extern struct thread *running_thread;
extern struct process *running_process;

void return_from_interrupt(void);
void set_kernel_stack(void *);

void threads_init(void);

struct process *process_by_id(pid_t pid);
struct thread *thread_by_id(pid_t tid);

struct process *bootstrap_usermode(const char *init_filename);
struct process *new_user_process(uintptr_t entrypoint);

struct thread *new_thread(void);
struct thread *new_kthread(uintptr_t entrypoint);

struct thread *thread_sched(void);

void thread_block(void);
void thread_yield(void);
void thread_timeout(void);
void thread_done(void);

void thread_switch(struct thread *new, struct thread *old);

noreturn void exit_kthread(void);
noreturn void do_thread_exit(int exit_status, enum thread_state state);
noreturn void do_process_exit(int exit_status);

void block_thread(struct list *threads);
void wake_blocked_thread(struct thread *th);
void wake_blocked_threads(struct list *threads);
void wake_process_thread(struct process *p);

void kill_process_group(pid_t pgid);
void kill_process(struct process *p);
void kill_pid(pid_t pid);

void thread_enqueue(struct thread *);
void thread_enqueue_at_front(struct thread *);
void drop_thread(struct thread *);
struct thread *process_thread(struct process *);

#endif // NG_THREAD_H

