#include <assert.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/mt/process.h>
#include <ng/mt/thread.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/trace.h>

extern "C" {

static void wake_tracer_with(struct thread *tracee, int value);

static bool is_stopped(struct thread *th) { return th->state == TS_TRWAIT; }

static sysret trace_traceme()
{
    struct process *parent = running_process->parent;
    struct thread *parent_th = process_thread(parent);
    running_thread->trace_state = TRACE_RUNNING;
    running_thread->tracer = parent_th;
    parent_th->tracees.push_back(*running_addr());
    return 0;
}

static sysret trace_attach(struct thread *th)
{
    if (!th)
        return -ESRCH;
    th->tracer = running_addr();
    th->trace_state = TRACE_RUNNING;
    running_thread->tracees.push_back(*th);
    return 0;
}

static sysret trace_getregs(struct thread *th, void *data)
{
    if (!th)
        return -ESRCH;
    if (!is_stopped(th))
        return -EINVAL;
    memcpy(data, th->user_ctx, sizeof(interrupt_frame));
    return 0;
}

static sysret trace_setregs(struct thread *th, void *data)
{
    if (!th)
        return -ESRCH;
    if (!is_stopped(th))
        return -EINVAL;
    memcpy(th->user_ctx, data, sizeof(interrupt_frame));
    return 0;
}

static sysret trace_start(struct thread *th, enum trace_state ns, int signal)
{
    if (!th)
        return -ESRCH;
    bool should_start = is_stopped(th);
    bool in_signal = th->trace_state == TRACE_SIGNAL_DELIVERY_STOP;

    th->trace_state = ns;

    /* When you continue a traced thread, you may pass a signal that will
     * be delivered to that thread. If the thread is stopped in signal
     * delivery stop, it will live replace the recieved signal (or remove
     * it).
     * Otherwise, I just add the signal to the pending set. This may not
     * technically be the correct behavior if there are already pending
     * signals, but it sure beats longjmping directly to handle signal
     * out of nowhere, which is the only alternative that comes to mind.
     */
    if (in_signal) {
        th->trace_signal = signal;
    } else {
        if (signal)
            sigaddset(&th->sig_pending, signal);
    }

    if (ns == TRACE_SINGLESTEP) {
        th->user_ctx->flags |= TRAP_FLAG;
    } else {
        th->user_ctx->flags &= ~TRAP_FLAG;
    }

    if (should_start) {
        th->state = TS_RUNNING;
        thread_enqueue(th);
    }
    return 0;
}

static sysret trace_detach(struct thread *th)
{
    if (!th)
        return -ESRCH;
    th->tracer->tracees.remove(*th);
    th->tracer = nullptr;
    return trace_start(th, TRACE_RUNNING, 0);
}

sysret sys_trace(enum trace_command cmd, pid_t pid, void *addr, void *data)
{
    struct thread *th = thread_by_id(pid);
    int d_signal = (int)(intptr_t)data; // fun warnings

    switch (cmd) {
    case TR_TRACEME:
        return trace_traceme();
    case TR_ATTACH:
        return trace_attach(th);
    case TR_GETREGS:
        return trace_getregs(th, data);
    case TR_SETREGS:
        return trace_setregs(th, data);
    case TR_READMEM: // fallthrough
    case TR_WRITEMEM:
        return -ETODO;
    case TR_SINGLESTEP:
        return trace_start(th, TRACE_SINGLESTEP, d_signal);
    case TR_SYSCALL:
        return trace_start(th, TRACE_SYSCALL, d_signal);
    case TR_CONT:
        return trace_start(th, TRACE_RUNNING, d_signal);
    case TR_DETACH:
        return trace_detach(th);
    }
    return -EINVAL;
}

static void wake_tracer_with(struct thread *tracee, int value)
{
    struct thread *tracer = tracee->tracer;
    if (!tracer)
        return;

    tracee->trace_report = value;
    tracee->state = TS_TRWAIT;
    if (tracer->state == TS_WAIT
        && (tracer->wait_request == 0
            || tracer->wait_request == running_thread->tid)
        && !tracer->wait_trace_result) {
        tracer->state = TS_RUNNING;
        tracer->wait_trace_result = running_addr();
    }
    signal_send_th(tracee->tracer, SIGCHLD);
}

void trace_syscall_entry(struct thread *tracee, int syscall)
{
    if (tracee->trace_state == TRACE_RUNNING)
        return;

    int report = TRACE_SYSCALL_ENTRY | syscall;

    tracee->trace_state = TRACE_SYSCALL_ENTER_STOP;
    wake_tracer_with(tracee, report);
    thread_block();
}

void trace_syscall_exit(struct thread *tracee, int syscall)
{
    if (tracee->trace_state == TRACE_RUNNING)
        return;

    int report = TRACE_SYSCALL_EXIT | syscall;

    tracee->trace_state = TRACE_SYSCALL_EXIT_STOP;
    wake_tracer_with(tracee, report);
    thread_block();
}

int trace_signal_delivery(int signal, sighandler_t handler)
{
    struct thread *tracee = running_addr();
    if (!running_thread->tracer)
        return signal;
    int report = TRACE_SIGNAL | signal;

    tracee->trace_state = TRACE_SIGNAL_DELIVERY_STOP;
    wake_tracer_with(tracee, report);
    thread_block();

    return tracee->trace_signal;
}

void trace_report_trap(int interrupt)
{
    assert(running_thread->tracer);

    struct thread *tracee = running_addr();
    int report = TRACE_TRAP | interrupt;

    tracee->trace_state = TRACE_TRAPPED;
    wake_tracer_with(tracee, report);
    thread_block();
}

} // extern "C"