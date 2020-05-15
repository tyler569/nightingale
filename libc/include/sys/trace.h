
#pragma once
#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_

#include <sys/types.h>

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

int trace(pid_t pid, enum trace_command cmd, void *addr, void *data);

#endif // _SYS_TRACE_H_
