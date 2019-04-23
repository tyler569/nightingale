
#include <ng/basic.h>
#include "errno.h"
#include <ng_syscall.h>
#include <stdio.h>

// TODO: errno should be thread-local
int errno;

const char *const perror_strings[] = {
    [SUCCESS] = "No error",
    [EINVAL] = "(EINVAL) Invalid argument",
    [EWOULDBLOCK] = "(EAGAIN) Would block",
    [ENOEXEC] = "(ENOEXEC) Argument is not executable",
    [ENOENT] = "(ENOENT) Entity does not exist",
    [EAFNOSUPPORT] = "(EAFNOSUPPORT) Unsupported protocol",
    [ECHILD] = "(ECHILD) No such child",
    [EPERM] = "(EPERM) No permission",
    [EFAULT] = "(EFAULT) Fault occurred",
    [EBADF] = "(EBADF) Bad fd",
};

void perror(const char *const message) {
        printf("%s: %s\n", message, perror_strings[errno]);
}

const char *strerror(int errno) { return perror_strings[errno]; }
