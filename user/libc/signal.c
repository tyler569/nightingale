
#include <ng/basic.h>
#include <stddef.h>
#include <signal.h>

sighandler_t signal(int signum, sighandler_t handler) {
    // noop, this does nothing
    return NULL;
}

