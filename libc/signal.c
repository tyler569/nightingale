#include "ng/common.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigemptyset(sigset_t *set)
{
    *set = 0;
    return 0;
}

int sigfillset(sigset_t *set)
{
    *set = -1;
    return 0;
}

int sigaddset(sigset_t *set, int signum)
{
    *set |= (1u << signum);
    return 0;
}

int sigdelset(sigset_t *set, int signum)
{
    *set &= ~(1u << signum);
    return 0;
}

int sigismember(const sigset_t *set, int signum)
{
    return (*set & (1u << signum)) != 0;
}

#ifndef __kernel__

sighandler_t signal(int signum, sighandler_t handler)
{
    return sigaction(signum, handler, 0);
}

int raise(int signal) { return kill(getpid(), signal); }

#endif
