
#include <basic.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/trace.h>
#include <nc/errno.h>

sysret sys_trace(pid_t pid, enum trace_command cmd, void *addr, void *data) {
        switch (cmd) {
        case TR_TRACEME: {
                struct process *parent = running_process->parent;
                struct thread *parent_th = process_thread(parent);
                running_thread->tracer = parent_th;
                running_thread->trace_state = TRACE_STOPPED;
                thread_block();
                return 0;
        } break;
        case TR_ATTACH: {
                struct thread *th = thread_by_id(pid);
                th->tracer = running_thread;
                th->trace_state = TRACE_RUNNING;
                return 0;
        } break;
        case TR_GETREGS: {
                struct thread *th = thread_by_id(pid);
                assert(th->trace_state == TRACE_STOPPED);
                memcpy(data, th->trace_frame, sizeof(interrupt_frame));
                return 0;
        } break;
        case TR_SETREGS: {
                struct thread *th = thread_by_id(pid);
                assert(th->trace_state == TRACE_STOPPED);
                memcpy(th->trace_frame, data, sizeof(interrupt_frame));
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
                bool was_stopped = th->trace_state == TRACE_STOPPED;
                th->trace_state = TRACE_SYSCALL;
                if (was_stopped) thread_enqueue(th);
                return 0;
        } break;
        case TR_CONT: {
                struct thread *th = thread_by_id(pid);
                bool was_stopped = th->trace_state == TRACE_STOPPED;
                th->trace_state = TRACE_RUNNING;
                if (was_stopped) thread_enqueue(th);
                return 0;
        } break;
        case TR_DETACH: {
                struct thread *th = thread_by_id(pid);
                bool was_stopped = th->trace_state == TRACE_STOPPED;
                th->trace_state = TRACE_RUNNING;
                th->tracer = NULL;
                if (was_stopped) thread_enqueue(th);
                return 0;
        } break;
        }
        return -EINVAL;
}

#define TRACE_SYSCALL_ENTRY 1
#define TRACE_SYSCALL_EXIT  2
#define TRACE_SIGNAL        3

void trace_wake_tracer_with(struct thread *tracee, int value) {
}

void trace_syscall_entry(struct thread *tracee, interrupt_frame *r) {
        if (tracee->trace_state == TRACE_RUNNING) {
                return;
        }
        tracee->trace_frame = r;

        trace_wake_tracer_with(tracee, TRACE_SYSCALL_ENTRY);
        thread_block();
}

void trace_syscall_exit(struct thread *tracee, interrupt_frame *r) {
        return;
}

int trace_signal_delivery(int signal, sighandler_t handler) {
        if (!running_thread->tracer) {
                return TRACE_SIGNAL_CONTINUE;
        }
        return TRACE_SIGNAL_CONTINUE;
}

