
#include <basic.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/trace.h>
#include <errno.h>

static void wake_tracer_with(struct thread *tracee, int value);

sysret sys_trace(pid_t pid, enum trace_command cmd, void *addr, void *data) {
        switch (cmd) {
        case TR_TRACEME: {
                struct process *parent = running_process->parent;
                struct thread *parent_th = process_thread(parent);
                running_thread->tracer = parent_th;
                running_thread->trace_state = TRACE_STOPPED;
                running_thread->state = TS_TRWAIT;
                _list_append(&parent_th->tracees, &running_thread->trace_node);
                wake_tracer_with(running_thread, TRACE_NEW_TRACEE);
                thread_block();
                return 0;
        } break;
        case TR_ATTACH: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                th->tracer = running_thread;
                th->trace_state = TRACE_RUNNING;
                _list_append(&running_thread->tracees, &th->trace_node);
                return 0;
        } break;
        case TR_GETREGS: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                assert(th->trace_state == TRACE_STOPPED);
                memcpy(data, th->user_ctx, sizeof(interrupt_frame));
                return 0;
        } break;
        case TR_SETREGS: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                assert(th->trace_state == TRACE_STOPPED);
                memcpy(th->user_ctx, data, sizeof(interrupt_frame));
                return 0;
        } break;
        case TR_READMEM: {
                return -ETODO;
        } break;
        case TR_WRITEMEM: {
                return -ETODO;
        } break;
        case TR_SINGLESTEP: {
                return -ETODO;
        } break;
        case TR_SYSCALL: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                bool was_stopped = th->state == TS_TRWAIT;
                th->trace_state = TRACE_SYSCALL;
                if (was_stopped) {
                        th->state = TS_RUNNING;
                        thread_enqueue(th);
                }
                return 0;
        } break;
        case TR_CONT: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                bool was_stopped = th->state == TS_TRWAIT;
                th->trace_state = TRACE_RUNNING;
                if (was_stopped) {
                        th->state = TS_RUNNING;
                        thread_enqueue(th);
                }
                return 0;
        } break;
        case TR_DETACH: {
                struct thread *th = thread_by_id(pid);
                if (!th)  return -ESRCH;
                bool was_stopped = th->state == TS_TRWAIT;
                th->trace_state = TRACE_RUNNING;
                th->tracer = NULL;
                if (was_stopped) {
                        th->state = TS_RUNNING;
                        thread_enqueue(th);
                }
                return 0;
        } break;
        }
        return -EINVAL;
}

static void wake_tracer_with(struct thread *tracee, int value) {
        struct thread *tracer = tracee->tracer;
        if (!tracer)  return;

        tracee->trace_report = value;
        tracee->trace_state = TRACE_STOPPED;
        tracee->state = TS_TRWAIT;
        if (
                tracer->state == TS_WAIT &&
                (tracer->wait_request == 0 ||
                 tracer->wait_request == running_thread->tid) &&
                !tracer->wait_trace_result
        ) {
                tracer->state = TS_RUNNING;
                tracer->wait_trace_result = running_thread;
        }
        signal_send_th(tracee->tracer, SIGCHLD);
}

void trace_syscall_entry(struct thread *tracee, int syscall) {
        if (tracee->trace_state == TRACE_RUNNING) {
                return;
        }

        int report = TRACE_SYSCALL_ENTRY | syscall;

        wake_tracer_with(tracee, report);
        thread_block();
}

void trace_syscall_exit(struct thread *tracee, int syscall) {
        if (tracee->trace_state == TRACE_RUNNING) {
                return;
        }

        int report = TRACE_SYSCALL_EXIT | syscall;

        wake_tracer_with(tracee, report);
        thread_block();
}

int trace_signal_delivery(int signal, sighandler_t handler) {
        if (!running_thread->tracer) {
                return TRACE_SIGNAL_CONTINUE;
        }
        return TRACE_SIGNAL_CONTINUE;
}

