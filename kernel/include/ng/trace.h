
#pragma once
#ifndef NG_TRACE_H
#define NG_TRACE_H

#include <nc/sys/trace.h>

enum trace_state {
        TRACE_RUNNING,

        TRACE_SYSCALL,
        TRACE_SYSEMU,
        TRACE_SINGLESTEP,

        TRACE_STOPPED,
};

#define TRACE_SIGNAL_CONTINUE 0
#define TRACE_SIGNAL_SUPPRESS 1

void trace_syscall_entry(struct thread *tracee, interrupt_frame *r);
void trace_syscall_exit(struct thread *tracee, interrupt_frame *r);
int trace_signal_delivery(int signal, sighandler_t);

#endif // NG_TRACE_H

