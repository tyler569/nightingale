
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include <thread.h>
#include <vmm.h>
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

const char* const syscall_debuginfos[] = {
    [SYS_EXIT]          = "exit(%zi)",
    [SYS_OPEN]          = "open_is_invalid()",
    [SYS_READ]          = "read(%zi, %p, %zu)",
    [SYS_WRITE]         = "write(%zi, %p, %zu)",
    [SYS_FORK]          = "fork()",
    [SYS_TOP]           = "top()",
    [SYS_GETPID]        = "getpid()",
    [SYS_GETTID]        = "gettid()",
    [SYS_EXECVE]        = "execve(%s, %p, %p)",
    [SYS_WAIT4]         = "wait4(%zi)",
    [SYS_SOCKET]        = "socket(%zi, %zi, %zi)",
    [SYS_STRACE]        = "strace(%zi)",
    [SYS_BIND]          = "bind(%zi, %p, %zu)",
    [SYS_CONNECT]       = "connect(%zi, %p, %zu)",
    [SYS_SEND]          = "send(%zi, %p, %zu, %zi)",
    [SYS_SENDTO]        = "sendto(%zi, %p, %zu, %zi, %p, %zu)",
    [SYS_RECV]          = "recv(%zi, %p, %zu, %zi)",
    [SYS_RECVFROM]      = "recvfrom(%zi, %p, %zu, %zi, %p, %p)",
    [SYS_WAITPID]       = "waitpid(%zi, %p, %#zx)",
    [SYS_DUP2]          = "dup2(%zi, %zi)"
};

const unsigned int syscall_ptr_mask[] = {
    [SYS_EXIT]          = 0, 
    [SYS_OPEN]          = 0,
    [SYS_READ]          = 0x02,
    [SYS_WRITE]         = 0x02,
    [SYS_FORK]          = 0,
    [SYS_TOP]           = 0,
    [SYS_GETPID]        = 0,
    [SYS_GETTID]        = 0,
    [SYS_EXECVE]        = 0x06,
    [SYS_WAIT4]         = 0,
    [SYS_SOCKET]        = 0,
    [SYS_STRACE]        = 0,
    [SYS_BIND]          = 0x02,
    [SYS_CONNECT]       = 0x02,
    [SYS_SEND]          = 0x02,
    [SYS_SENDTO]        = 0x12,
    [SYS_RECV]          = 0x02,
    [SYS_RECVFROM]      = 0x32,
    [SYS_WAITPID]       = 0x02,
    [SYS_DUP2]          = 0,
};

bool syscall_check_pointer(uintptr_t ptr) {
    uintptr_t resolved = vmm_resolve(ptr);
    if (resolved == ~0) {
        return false;
    }
    if (!(resolved & PAGE_USERMODE)) {
        return false;
    }
    return true;
}

#define check_ptr(enable, ptr) \
    if (enable && !syscall_check_pointer(ptr) && ptr != 0) { \
        struct syscall_ret ret = { 0, EFAULT }; \
        return ret; \
    }

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

    unsigned mask = syscall_ptr_mask[syscall_num];
    if (mask != 0) {
        check_ptr(mask & 0x01, arg1);
        check_ptr(mask & 0x02, arg2);
        check_ptr(mask & 0x04, arg3);
        check_ptr(mask & 0x08, arg4);
        check_ptr(mask & 0x10, arg5);
        check_ptr(mask & 0x20, arg6);
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

