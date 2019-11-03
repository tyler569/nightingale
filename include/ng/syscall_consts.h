
#pragma once
#ifndef NG_SYSCALL_CONSTS_H
#define NG_SYSCALL_CONSTS_H

enum {
        SUCCESS,
        EINVAL,
        EAGAIN,
        EWOULDBLOCK = EAGAIN,
        ENOEXEC,
        ENOENT,
        EAFNOSUPPORT,
        ECHILD,
        EPERM,
        EFAULT,
        EBADF,
        ERANGE,
        EDOM,
        ETODO,
};

enum {
        NG_INVALID,
        NG_DEBUGPRINT,
        NG_EXIT,
        NG_OPEN,
        NG_READ,
        NG_WRITE,
        NG_FORK,
        NG_TOP,
        NG_GETPID,
        NG_GETTID,
        NG_EXECVE,
        NG_WAIT4,
        NG_SOCKET,
        NG_BIND0,
        NG_CONNECT0,
        NG_STRACE,
        NG_BIND,
        NG_CONNECT,
        NG_SEND,
        NG_SENDTO,
        NG_RECV,
        NG_RECVFROM,
        NG_WAITPID,
        NG_DUP2,
        NG_UNAME,
        NG_YIELD,
        NG_SEEK,
        NG_POLL,
        NG_MMAP,
        NG_MUNMAP,
        NG_HEAPDBG,
        NG_SETPGID,
        NG_EXIT_GROUP,
        NG_CLONE0,
        NG_LOADMOD,
        NG_HALTVM,
        NG_OPENAT,
        NG_EXECVEAT,
        NG_TTYCTL,
        NG_CLOSE,

        SYSCALL_MAX,
};

/* TODO: come up with a better way to have these here without
 * needing to spread all of this all over the codebase
 */

/* waitpid flags */
#define WNOHANG 1

/* open flags */
#define O_RDONLY 0x0001
#define O_WRONLY 0x0002
#define O_RDWR   (O_RDONLY | O_WRONLY)

/* tty operations */
enum tty_ttyctls {
        TTY_SETPGRP,
        TTY_SETBUFFER,
        TTY_SETECHO,
};

#endif // NG_SYSCALL_CONSTS_H

