
#pragma once
#ifndef _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_
#define _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_

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
        ETODO,
};

enum {
        SYS_INVALID,
        SYS_DEBUGPRINT,
        SYS_EXIT,
        SYS_OPEN,
        SYS_READ,
        SYS_WRITE,
        SYS_FORK,
        SYS_TOP,
        SYS_GETPID,
        SYS_GETTID,
        SYS_EXECVE,
        SYS_WAIT4,
        SYS_SOCKET,
        SYS_BIND0,
        SYS_CONNECT0,
        SYS_STRACE,
        SYS_BIND,
        SYS_CONNECT,
        SYS_SEND,
        SYS_SENDTO,
        SYS_RECV,
        SYS_RECVFROM,
        SYS_WAITPID,
        SYS_DUP2,
        SYS_UNAME,
        SYS_YIELD,
        SYS_SEEK,
        SYS_POLL,
        SYS_MMAP,
        SYS_MUNMAP,
        SYS_HEAPDBG,
        SYS_SETPGID,
        SYS_EXIT_GROUP,
        SYS_CLONE0,
        SYS_LOADMOD,

        SYSCALL_MAX,
};

/* waitpid flags */
#define WNOHANG 1

#endif
