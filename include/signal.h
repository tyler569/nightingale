#pragma once
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#define SIG_DFL (sighandler_t)0
#define SIG_IGN (sighandler_t)2

#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

BEGIN_DECLS

enum signal {
	SIGABRT,
	SIGALRM,
	SIGBUS,
	SIGCHLD,
	SIGCONT,
	SIGFPE,
	SIGHUP,
	SIGILL,
	SIGINFO,
	SIGINT,
	SIGKILL,
	SIGPIPE,
	SIGPROF,
	SIGQUIT,
	SIGSEGV,
	SIGSTOP,
	SIGTSTP,
	SIGSYS,
	SIGTERM,
	SIGTRAP,
	SIGTTIN,
	SIGTTOU,
	SIGURG,
	SIGUSR1,
	SIGUSR2,
	SIGVTALRM,
	SIGXCPU,
	SIGWINCH,
};

typedef uint32_t sigset_t;
typedef void (*sighandler_t)(int);
typedef atomic_int sig_atomic_t;

// sigset operations
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

#ifndef __kernel__

int sigprocmask(int op, const sigset_t *new, sigset_t *old);
sighandler_t sigaction(int signum, sighandler_t handler, int flags);
sighandler_t signal(int signum, sighandler_t handler);
int kill(pid_t pid, int sig);
int raise(int signal);

#endif // __kernel__

END_DECLS

#endif // _SIGNAL_H_
