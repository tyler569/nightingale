#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

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

#define TRACE_SIGNAL_CONTINUE 0
#define TRACE_SIGNAL_SUPPRESS 1

#define TRACE_SYSCALL_ENTRY (1 << 16)
#define TRACE_SYSCALL_EXIT (2 << 16)
#define TRACE_SIGNAL (3 << 16)
#define TRACE_NEW_TRACEE (4 << 16)
#define TRACE_TRAP (5 << 16)

int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);

END_DECLS
