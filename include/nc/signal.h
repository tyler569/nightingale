
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <basic.h>
#include <stdatomic.h>
#include <stddef.h>

#define SIG_DFL (sighandler_t)0x01
#define SIGINT 2

typedef void (*sighandler_t)(int);
typedef atomic_int sig_atomic_t;

sighandler_t signal(int signum, sighandler_t handler);

#endif
