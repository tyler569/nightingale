
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <basic.h>
#include <stddef.h>

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);

static inline sighandler_t signal(int signum, sighandler_t handler) {
    // noop, this does nothing
    return NULL;
}

#endif

