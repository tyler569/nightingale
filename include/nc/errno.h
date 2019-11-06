
#pragma once
#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <ng/syscall_consts.h>
#include <stdio.h>

enum errno_values {
        SUCCESS,
        EINVAL,
        EAGAIN,
        EWOULDBLOCK = EAGAIN,
        ENOEXEC,
        ENOENT,
        EAFNOSUPPORT,
        EPROTONOSUPPORT,
        ECHILD,
        EPERM,
        EFAULT,
        EBADF,
        ERANGE,
        EDOM,
        ETODO,
};

extern const char *errno_names[];

#ifndef _NG

// TODO: errno should be thread-local
extern int errno;

void perror(const char *const message);
const char *strerror(int errno);

#endif // _NG

#endif // _ERRNO_H_

