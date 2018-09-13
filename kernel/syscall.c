
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
const void* const syscall_table[] = {
    [SYS_DEBUGPRINT] = NULL,    // deprecated
    [SYS_EXIT] = sys_exit,
    [SYS_OPEN] = NULL,          // unimplemented
    [SYS_READ] = sys_read,
    [SYS_WRITE] = sys_write,
    [SYS_FORK] = sys_fork,
    [SYS_TOP] = sys_top,
    [SYS_GETPID] = sys_getpid,
    [SYS_GETTID] = sys_gettid,
    [SYS_EXECVE] = sys_execve,
    //[SYS_WAIT4] = sys_wait4,    // temporarily deprecated
    [SYS_SOCKET] = sys_socket,
    [SYS_BIND0] = NULL,         // removed
    [SYS_CONNECT0] = NULL,      // removed
    [SYS_STRACE] = sys_strace,
    [SYS_BIND] = sys_bind,
    [SYS_CONNECT] = sys_connect,
    [SYS_SEND] = sys_send,
    [SYS_SENDTO] = sys_sendto,
    [SYS_RECV] = sys_recv,
    [SYS_RECVFROM] = sys_recvfrom,
    [SYS_WAITPID] = sys_waitpid,
    [SYS_DUP2] = sys_dup2,
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

    if (running_thread->strace) {
        printf(syscall_debuginfos[syscall_num],
               arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    const syscall_t* call = (const syscall_t*)syscall_table[syscall_num];
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

