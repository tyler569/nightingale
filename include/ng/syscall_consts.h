
#pragma once
#ifndef NG_SYSCALL_CONSTS_H
#define NG_SYSCALL_CONSTS_H

enum ng_syscall {
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
        NG_PIPE,
        NG_SIGACTION,
        NG_SIGRETURN,
        NG_KILL,
        NG_SLEEPMS,
        NG_GETDIRENTS,

        SYSCALL_MAX,
};

#endif // NG_SYSCALL_CONSTS_H

