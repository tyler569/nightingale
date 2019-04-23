
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <ng/basic.h>
#include <stdatomic.h>
#include <stddef.h>

typedef void (*sighandler_t)(int);
typedef atomic_int sig_atomic_t;

sighandler_t signal(int signum, sighandler_t handler);

#endif
