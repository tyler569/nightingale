
#include <ng/basic.h>
#include <signal.h>
#include <stddef.h>

sighandler_t signal(int signum, sighandler_t handler) {
        // noop, this does nothing
        return NULL;
}
