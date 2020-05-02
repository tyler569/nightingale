
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

void trace_syscall_entry(struct thread *tracee, interrupt_frame *r);
void trace_syscall_exit(struct thread *tracee, interrupt_frame *r);

#endif // NG_TRACE_H

