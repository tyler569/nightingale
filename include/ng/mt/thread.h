#pragma once
#ifndef NG_MT_THREAD_H
#define NG_MT_THREAD_H

#include <ng/thread.h>
#include <nx/atomic.h>
#include <nx/list.h>

constexpr int thread_magic = 0x74687264; // "thrd"

struct thread {
    pid_t tid;
    process *proc;

    unsigned magic { thread_magic };

    thread_state state { TS_PREINIT };
    thread_flags flags {};
    thread_state nonsig_state {}; // original state before signal

    char *kstack { nullptr };

    jmp_buf kernel_ctx {};
    interrupt_frame *user_ctx {};

    void (*entry)(void *);
    void *entry_arg;

    dentry *cwd { global_root_dentry };
    dentry *proc_dir { nullptr };

    pid_t wait_request {};
    struct process *wait_result { nullptr };
    struct thread *wait_trace_result { nullptr };

    nx::list_node trace_node;
    nx::list<thread, &thread::trace_node> tracees {};
    thread *tracer { nullptr };
    trace_state trace_state {};
    int trace_report {};
    int trace_signal {};

    uint64_t report_events {};

    nx::list_node all_threads;
    nx::list_node runnable;
    nx::list_node freeable;
    nx::list_node process_threads;

    struct timer_event *wait_event { nullptr };

    uintptr_t user_sp {};
    jmp_buf signal_ctx {};

    sighandler_t sig_handlers[32] {};
    sigset_t sig_pending {};
    sigset_t sig_mask {};

    long n_scheduled {};

    // in kernel_ticks
    uint64_t time_ran {};
    uint64_t last_scheduled {};

    // in tsc time - divide by tsc_average_delta (TODO) -- kernel/timer
    uint64_t tsc_ran {};
    uint64_t tsc_scheduled {};

    int irq_disable_depth {};

    int awaiting_newmutex {};
    int awaiting_deli_ticket {};

    void *tlsbase {};

    fp_ctx fpctx {};

    thread(pid_t tid, process *proc, void (*entry)(void *), void *entry_arg)
        : tid(tid)
        , proc(proc)
        , entry(entry)
        , entry_arg(entry_arg)
    {
    }
};

extern nx::list<thread, &thread::all_threads> all_threads;

#endif // NG_MT_THREAD_H
