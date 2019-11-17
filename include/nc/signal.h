
#pragma once
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <basic.h>
#include <stddef.h>
#include <sys/types.h>

#define SIG_DFL (sighandler_t)0
#define SIG_IGN (sighandler_t)2

enum signals {
        SIGSEGV = 1,
        SIGINT  = 2,
};

typedef void (*sighandler_t)(int);
typedef atomic_int sig_atomic_t;

sighandler_t sigaction(int signum, sighandler_t handler, int flags);
sighandler_t signal(int signum, sighandler_t handler);
int kill(pid_t pid, int sig);

#endif // _SIGNAL_H_

