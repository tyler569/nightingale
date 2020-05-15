
#pragma once
#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <stdio.h>

enum errno_value {
        SUCCESS,
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
        ETODO,

        EWOULDBLOCK = EAGAIN,
};

extern const char *errno_names[];

#ifdef _NG
#define is_error(R) ((intptr_t)(R) < 0 && (intptr_t)(R) > -0x1000)
#endif // _NG



#ifndef _NG

// TODO: errno should be thread-local
extern enum errno_value errno;

void perror(const char *const message);
const char *strerror(enum errno_value errno);

#endif // _NG

#endif // _ERRNO_H_

