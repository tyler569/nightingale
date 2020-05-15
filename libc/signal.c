
#include <basic.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigemptyset(sigset_t *set) {
        return *set = (sigset_t)0;
}

int sigfillset(sigset_t *set) {
        return *set = (sigset_t)~0;
}

int sigaddset(sigset_t *set, int signum) {
        return *set |= (1 << signum);
}

int sigdelset(sigset_t *set, int signum) {
        return *set &= ~(1 << signum);
}

int sigismember(const sigset_t *set, int signum) {
        return (*set & (1 << signum)) != 0;
}

#ifndef __kernel__

sighandler_t signal(int signum, sighandler_t handler) {
        return sigaction(signum, handler, 0);
}

int raise(int signal) {
        return kill(getpid(), signal);
}

#endif

