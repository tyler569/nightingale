
#pragma once
#ifndef NG_TRACE_H
#define NG_TRACE_H

enum trace_status {
        TRACE_SYSCALL,
        TRACE_SYSEMU,
        TRACE_SINGLESTEP,
};

enum trace_command {
        TR_TRAECME,
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

#endif // NG_TRACE_H

