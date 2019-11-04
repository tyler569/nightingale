
#include <basic.h>
#include <signal.h>
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>

sighandler_t signal(int signum, sighandler_t handler) {
        errno = ETODO;
        return NULL;
}

int kill(pid_t pid, int sig) {
        errno = ETODO;
        return 0;
}

