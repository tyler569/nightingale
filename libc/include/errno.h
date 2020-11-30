#pragma once
#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <stdio.h>

enum errno_value {
    SUCCESS,
    ETODO,
    EINVAL,
    EAGAIN,
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
    EACCES,
    ESPIPE,
    EISDIR,
    ENOMEM,
    EINTR,
    ESRCH,
    ENOSYS,
    ENOTTY,
    ENOTDIR,
    ECONNREFUSED,
    ENODEV,
    EOPNOTSUPP,
    ENOTSOCK,
    EEXIST,

    ERRNO_MAX
};

#define EWOULDBLOCK EAGAIN

extern const char *errno_names[];

#ifndef _NG
extern int errno;
void perror(const char *const message);
const char *strerror(enum errno_value errno);
#endif // _NG

#endif // _ERRNO_H_
