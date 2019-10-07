
#include <ng/basic.h>
#include "errno.h"
#include <ng/syscall_consts.h>
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
        [ETODO] = "(ETODO) Work in progress",
};

void perror(const char *const message) {
        if (errno >= 0 && errno <= ETODO) {
                printf("%s: %s\n", message, perror_strings[errno]);
        } else {
                printf("%s: Unknown Error (%i)\n", message, errno);
        }
}

const char *strerror(int errno) { return perror_strings[errno]; }
