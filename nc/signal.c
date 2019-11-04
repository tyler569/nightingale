
#include <basic.h>
#include <signal.h>
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>

sighandler_t signal(int signum, sighandler_t handler) {
        return sigaction(signum, handler, 0);
}

