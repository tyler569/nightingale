#pragma once
#ifndef NG_TRACE_H
#define NG_TRACE_H

#include <signal.h>
#include <sys/cdefs.h>

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

#ifdef __kernel__

enum trace_state {
	TRACE_RUNNING,

	TRACE_SYSCALL,
	TRACE_SYSEMU,
	TRACE_SINGLESTEP,

	TRACE_SYSCALL_ENTER_STOP,
	TRACE_SYSCALL_EXIT_STOP,
	TRACE_SIGNAL_DELIVERY_STOP,
	TRACE_TRAPPED,
};

struct thread;

void trace_syscall_entry(struct thread *tracee, int syscall);
void trace_syscall_exit(struct thread *tracee, int syscall);
int trace_signal_delivery(int signal, sighandler_t);
void trace_report_trap(int interrupt);

#endif

END_DECLS

#endif // NG_TRACE_H
