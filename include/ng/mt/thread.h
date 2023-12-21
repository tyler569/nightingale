#pragma once
#ifndef NG_MT_THREAD_H
#define NG_MT_THREAD_H

#include <ng/thread.h>
#include <nx/atomic.h>
#include <nx/functional.h>
#include <nx/list.h>
#include <nx/optional.h>
#include <nx/stddef.h>

constexpr int thread_magic = 0x74687264; // "thrd"

struct bootstrap_thread { };

struct thread {
    pid_t tid {};
    process *proc {};

    unsigned magic { thread_magic };

    thread_state state { TS_PREINIT };
    thread_flags flags {};
    thread_state nonsig_state {}; // original state before signal

    char *kstack {};

    jmp_buf kernel_ctx {};
    interrupt_frame *user_ctx {};

    nx::optional<nx::function<int()>> m_entry_fn {};

    dentry *cwd {};
    dentry *proc_dir {};

    pid_t wait_request {};
    struct process *wait_result {};
    struct thread *wait_trace_result {};

    nx::list_node trace_node;
    nx::list<thread, &thread::trace_node> tracees {};
    thread *tracer {};
    trace_state trace_state {};
    int trace_report {};
    int trace_signal {};

    uint64_t report_events {};

    nx::list_node all_threads;
    nx::list_node runnable;
    nx::list_node freeable;
    nx::list_node process_threads;

    struct timer_event *wait_event {};

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

    int irq_disable_depth { 1 };

    int awaiting_newmutex {};
    int awaiting_deli_ticket {};

    int awaiting_newnewmutex {};
    unsigned awaiting_newnew_deli_ticket {};

    void *tlsbase {};

    fp_ctx fpctx {};

    constexpr thread() = default;
    constexpr thread(bootstrap_thread, process *proc)
        : proc(proc)
    {
    }
    // constexpr thread(process *, int);
    // explicit thread(nx::nullptr_t);
    // thread(nx::nullptr_t, nx::function<int()> &&entry);
    // thread(nx::nullptr_t, void (*entry)(void *), void *entry_arg);
    // explicit thread(process *proc);
    // thread(process *proc, nx::function<int()> &&entry);
    // thread(process *proc, void (*entry)(void *), void *entry_arg);

    static thread *spawn_generic();

    template <class F>
        requires requires(F f) { nx::function<int()> { nx::move(f) }; }
    static thread *spawn_kernel(F &&f);

    static thread *spawn_user(const char *filename);

    static thread *spawn_from_clone(
        interrupt_frame *, void *new_stack, int (*fn)(void *), void *arg);
    static thread *spawn_from_fork(interrupt_frame *);

    static thread &current() { return *this_cpu->running; }

    // template <class F>
    //     requires requires(F f) { nx::function<int()> { f }; }
    // thread(pid_t tid, process *proc, F entry)
    //     : tid(tid)
    //     , proc(proc)
    //     , m_entry_fn(nx::function<int()> { entry })
    // {
    // }

    void add_flag(thread_flags flag)
    {
        flags = static_cast<thread_flags>(flags | flag);
    }

    void remove_flag(thread_flags flag)
    {
        flags = static_cast<thread_flags>(flags & ~flag);
    }

    bool has_flag(thread_flags flag) const { return flags & flag; }

    void start()
    {
        state = TS_STARTED;
        thread_enqueue(this);
    }

    void attach(process *p);
};

extern nx::list<thread, &thread::all_threads> all_threads;

#endif // NG_MT_THREAD_H
