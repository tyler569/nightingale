
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include <thread.h>
#include <fs/vfs.h>
#include <arch/x86/cpu.h>
#include "syscall.h"

#include <syscalls.h> // syscall sys_* prototypes

#include <ng_syscall.h> // will this stay here?

// TODO: use this table
typedef struct syscall_ret syscall_t();
const uintptr_t syscall_table[] = {
    [SYS_DEBUGPRINT]    = 0,                        // deprecated
    [SYS_EXIT]          = (uintptr_t) sys_exit,
    [SYS_OPEN]          = 0,                        // unimplemented
    [SYS_READ]          = (uintptr_t) sys_read,
    [SYS_WRITE]         = (uintptr_t) sys_write,
    [SYS_FORK]          = (uintptr_t) sys_fork,
    [SYS_TOP]           = (uintptr_t) sys_top,
    [SYS_GETPID]        = (uintptr_t) sys_getpid,
    [SYS_GETTID]        = (uintptr_t) sys_gettid,
    [SYS_EXECVE]        = (uintptr_t) sys_execve,
    [SYS_WAIT4]         = 0,                        // temporarily deprecated
    [SYS_SOCKET]        = (uintptr_t) sys_socket,
    [SYS_BIND0]         = 0,                        // removed
    [SYS_CONNECT0]      = 0,                        // removed
    [SYS_STRACE]        = (uintptr_t) sys_strace,
    [SYS_BIND]          = (uintptr_t) sys_bind,
    [SYS_CONNECT]       = (uintptr_t) sys_connect,
    [SYS_SEND]          = (uintptr_t) sys_send,
    [SYS_SENDTO]        = (uintptr_t) sys_sendto,
    [SYS_RECV]          = (uintptr_t) sys_recv,
    [SYS_RECVFROM]      = (uintptr_t) sys_recvfrom,
    [SYS_WAITPID]       = (uintptr_t) sys_waitpid,
    [SYS_DUP2]          = (uintptr_t) sys_dup2,
};

const char *const syscall_debuginfos[] = {
    [SYS_EXIT] = "exit(%li)",
    [SYS_OPEN] = "open_is_invalid()",
    [SYS_READ] = "read(%li, %#lx, %lu)",
    [SYS_WRITE] = "write(%li, %#lx, %lu)",
    [SYS_FORK] = "fork()",
    [SYS_TOP] = "top()",
    [SYS_GETPID] = "getpid()",
    [SYS_GETTID] = "gettid()",
    [SYS_EXECVE] = "execve(%s, %#lx, %#lx)",
    [SYS_WAIT4] = "wait4(%li)",
    [SYS_SOCKET] = "socket(%li, %li, %li)",
    [SYS_STRACE] = "strace(%li)",
    [SYS_BIND] = "bind(%li, %#lx, %lu)",
    [SYS_CONNECT] = "connect(%li, %#lx, %lu)",
    [SYS_SEND] = "send(%li, %#lx, %lu, %li)",
    [SYS_SENDTO] = "sendto(%li, %#lx, %lu, %li, %#lx, %lu)",
    [SYS_RECV] = "recv(%li, %#lx, %lu, %li)",
    [SYS_RECVFROM] = "recvfrom(%li, %#lx, %lu, %li, %#lx, %#lx)",
    [SYS_WAITPID] = "waitpid(%li, %#lx, %#lx)",
    [SYS_DUP2] = "dup2(%li, %li)"
};

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
struct syscall_ret do_syscall_with_table(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame) {
    
    if (syscall_num > SYSCALL_MAX || syscall_num <= SYS_INVALID) {
        panic("invalid syscall number: %i\n", syscall_num);
    }
    if (syscall_table[syscall_num] == 0) {
        panic("invalid syscall number: %i, deprecated or removed\n", syscall_num);
    }

    if (running_thread->strace) {
        printf(syscall_debuginfos[syscall_num],
               arg1, arg2, arg3, arg4, arg5, arg6);
    }

    syscall_t* const call = (syscall_t* const)syscall_table[syscall_num];
    struct syscall_ret ret;

    if (syscall_num == SYS_EXECVE || syscall_num == SYS_FORK) {
        ret = call(frame, arg1, arg2, arg3, arg4, arg5, arg6);
    } else {
        ret = call(arg1, arg2, arg3, arg4, arg5, arg6);
    }

    if (running_thread->strace) {
        printf(" -> { %lu, %lu }\n", ret.value, ret.error);
    }

    return ret;
}

