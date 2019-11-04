
#pragma once
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <basic.h>
#include <stdatomic.h>
#include <stddef.h>
#include <sys/types.h>

#define SIG_DFL (sighandler_t)1
#define SIG_IGN (sighandler_t)2

enum signals {
        SIGINT = 2,
};

typedef void (*sighandler_t)(int);
typedef atomic_int sig_atomic_t;

sighandler_t signal(int signum, sighandler_t handler);
int kill(pid_t pid, int sig);

#endif // _SIGNAL_H_

