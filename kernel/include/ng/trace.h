#pragma once

#include <signal.h>
#include <sys/cdefs.h>
#include <sys/trace.h>

BEGIN_DECLS

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

END_DECLS
