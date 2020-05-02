
#pragma once
#ifndef NG_TRACE_H
#define NG_TRACE_H

enum trace_state {
        TRACE_RUNNING,

        TRACE_SYSCALL,
        TRACE_SYSEMU,
        TRACE_SINGLESTEP,

        TRACE_STOPPED,
};

enum trace_command {
        TR_TRACEME,
        TR_ATTACH,

        TR_GETREGS,
        TR_SETREGS,

        TR_READMEM,
        TR_WRITEMEM,

        TR_SINGLESTEP,
        TR_SYSCALL,
        TR_CONT,
        TR_DETACH,
};

void trace_syscall_entry(struct thread *tracee, interrupt_frame *r);
void trace_syscall_exit(struct thread *tracee, interrupt_frame *r);

#endif // NG_TRACE_H

