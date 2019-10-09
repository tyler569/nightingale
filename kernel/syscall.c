
#include <ng/basic.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h> // syscall sys_* prototypes
#include <ng/thread.h>
#include <ng/vmm.h>
#include <arch/cpu.h>
#include <ng/fs.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>

typedef struct syscall_ret syscall_t();
const uintptr_t syscall_table[] = {
        [SYS_DEBUGPRINT] = 0, // removed
        [SYS_EXIT]       = (uintptr_t) sys_exit,
        [SYS_OPEN]       = (uintptr_t) sys_open,
        [SYS_READ]       = (uintptr_t) sys_read,
        [SYS_WRITE]      = (uintptr_t) sys_write,
        [SYS_FORK]       = (uintptr_t) sys_fork,
        [SYS_TOP]        = (uintptr_t) sys_top,
        [SYS_GETPID]     = (uintptr_t) sys_getpid,
        [SYS_GETTID]     = (uintptr_t) sys_gettid,
        [SYS_EXECVE]     = (uintptr_t) sys_execve,
        [SYS_WAIT4]      = 0, // removed
        [SYS_SOCKET]     = 0, // (uintptr_t) sys_socket,
        [SYS_BIND0]      = 0, // removed
        [SYS_CONNECT0]   = 0, // removed
        [SYS_STRACE]     = (uintptr_t) sys_strace,
        [SYS_BIND]       = 0, // (uintptr_t) sys_bind,
        [SYS_CONNECT]    = 0, // (uintptr_t) sys_connect,
        [SYS_SEND]       = 0, // (uintptr_t) sys_send,
        [SYS_SENDTO]     = 0, // (uintptr_t) sys_sendto,
        [SYS_RECV]       = 0, // (uintptr_t) sys_recv,
        [SYS_RECVFROM]   = 0, // (uintptr_t) sys_recvfrom,
        [SYS_WAITPID]    = (uintptr_t) sys_waitpid,
        [SYS_DUP2]       = (uintptr_t) sys_dup2,
        [SYS_UNAME]      = (uintptr_t) sys_uname,
        [SYS_YIELD]      = (uintptr_t) sys_yield,
        [SYS_SEEK]       = (uintptr_t) sys_seek,
        [SYS_POLL]       = (uintptr_t) sys_poll,
        [SYS_MMAP]       = (uintptr_t) sys_mmap,
        [SYS_MUNMAP]     = (uintptr_t) sys_munmap,
        [SYS_HEAPDBG]    = (uintptr_t) sys_heapdbg,
        [SYS_SETPGID]    = (uintptr_t) sys_setpgid,
        [SYS_EXIT_GROUP] = (uintptr_t) sys_exit_group,
        [SYS_CLONE0]     = (uintptr_t) sys_clone0,
        [SYS_LOADMOD]    = (uintptr_t) sys_loadmod,
        [SYS_HALTVM]     = (uintptr_t) sys_haltvm,
        [SYS_OPENAT]     = (uintptr_t) sys_openat,
        [SYS_EXECVEAT]   = (uintptr_t) sys_execveat,
};

const char *const syscall_debuginfos[] = {
        [SYS_EXIT]       = "exit(%zi)",
        [SYS_OPEN]       = "open(\"%s\", %zi)",
        [SYS_READ]       = "read(%zi, %p, %zu)",
        [SYS_WRITE]      = "write(%zi, %p, %zu)",
        [SYS_TOP]        = "top(%zi)",
        [SYS_FORK]       = "fork()",
        [SYS_GETPID]     = "getpid()",
        [SYS_GETTID]     = "gettid()",
        [SYS_EXECVE]     = "execve(\"%s\", %p, %p)",
        [SYS_WAIT4]      = "wait4(%zi)",
        [SYS_SOCKET]     = "socket(%zi, %zi, %zi)",
        [SYS_STRACE]     = "strace(%zi)",
        [SYS_BIND]       = "bind(%zi, %p, %zu)",
        [SYS_CONNECT]    = "connect(%zi, %p, %zu)",
        [SYS_SEND]       = "send(%zi, %p, %zu, %zi)",
        [SYS_SENDTO]     = "sendto(%zi, %p, %zu, %zi, %p, %zu)",
        [SYS_RECV]       = "recv(%zi, %p, %zu, %zi)",
        [SYS_RECVFROM]   = "recvfrom(%zi, %p, %zu, %zi, %p, %p)",
        [SYS_WAITPID]    = "waitpid(%zi, %p, %#zx)",
        [SYS_DUP2]       = "dup2(%zi, %zi)",
        [SYS_UNAME]      = "uname(%p)",
        [SYS_YIELD]      = "yield()",
        [SYS_SEEK]       = "seek(%zi, %zi, %zi)",
        [SYS_POLL]       = "poll(%p, %zi, %zi)",
        [SYS_MMAP]       = "mmap(%p, %zu, %zi, %zi, %zi, %zi)",
        [SYS_MUNMAP]     = "munmap(%p, %zu)",
        [SYS_HEAPDBG]    = "heapdbg(%zi)",
        [SYS_SETPGID]    = "setpgid()",
        [SYS_EXIT_GROUP] = "exit_group(%zi)",
        [SYS_CLONE0]     = "clone0(%p, %p, %p, %zi)",
        [SYS_LOADMOD]    = "loadmod(%zi)",
        [SYS_HALTVM]     = "haltvm(%zi)",
        [SYS_EXECVEAT]   = "execveat(%zi, %s, %p, %p)",
};

const unsigned int syscall_ptr_mask[] = {
        [SYS_EXIT]       = 0,
        [SYS_OPEN]       = 0x01,
        [SYS_READ]       = 0x02,
        [SYS_WRITE]      = 0x02,
        [SYS_FORK]       = 0,
        [SYS_TOP]        = 0,
        [SYS_GETPID]     = 0,
        [SYS_GETTID]     = 0,
        [SYS_EXECVE]     = 0x06,
        [SYS_WAIT4]      = 0,
        [SYS_SOCKET]     = 0,
        [SYS_STRACE]     = 0,
        [SYS_BIND]       = 0x02,
        [SYS_CONNECT]    = 0x02,
        [SYS_SEND]       = 0x02,
        [SYS_SENDTO]     = 0x12,
        [SYS_RECV]       = 0x02,
        [SYS_RECVFROM]   = 0x32,
        [SYS_WAITPID]    = 0x02,
        [SYS_DUP2]       = 0,
        [SYS_UNAME]      = 0x01,
        [SYS_YIELD]      = 0,
        [SYS_SEEK]       = 0,
        [SYS_POLL]       = 0x01,
        [SYS_MMAP]       = 0x01,
        [SYS_MUNMAP]     = 0x01,
        [SYS_HEAPDBG]    = 0,
        [SYS_SETPGID]    = 0,
        [SYS_EXIT_GROUP] = 0,
        [SYS_CLONE0]     = 0x07,
        [SYS_LOADMOD]    = 0,
        [SYS_HALTVM]     = 0,
        [SYS_OPENAT]     = 0x02,
        [SYS_EXECVEAT]   = 0x0C,
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
        if (enable && ptr != 0 && !syscall_check_pointer(ptr)) { \
                struct syscall_ret ret = {0, EFAULT}; \
                return ret; \
        }

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
struct syscall_ret do_syscall_with_table(int syscall_num, intptr_t arg1,
                intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5,
                intptr_t arg6, interrupt_frame *frame) {

        if (syscall_num >= SYSCALL_MAX || syscall_num <= SYS_INVALID) {
                panic("invalid syscall number: %i\n", syscall_num);
        }

        unsigned int mask = syscall_ptr_mask[syscall_num];
        check_ptr(mask & 0x01, arg1);
        check_ptr(mask & 0x02, arg2);
        check_ptr(mask & 0x04, arg3);
        check_ptr(mask & 0x08, arg4);
        check_ptr(mask & 0x10, arg5);
        check_ptr(mask & 0x20, arg6);

        if (running_thread->flags & THREAD_STRACE) {
                printf(syscall_debuginfos[syscall_num],
                        arg1, arg2, arg3, arg4, arg5, arg6);
        }

        syscall_t *const call = (syscall_t *const)syscall_table[syscall_num];
        struct syscall_ret ret = {0};

        if (call == 0) {
                ret.error = EINVAL;
        } else {
                if (syscall_num == SYS_EXECVE || syscall_num == SYS_FORK ||
                    syscall_num == SYS_CLONE0) {
                        ret = call(frame, arg1, arg2, arg3, arg4, arg5, arg6);
                } else {
                        ret = call(arg1, arg2, arg3, arg4, arg5, arg6);
                }
        }

        if (running_thread->flags & THREAD_STRACE) {
                printf(" -> %s(%lu)\n", ret.error ? "error" : "value",
                       ret.error ? ret.error : ret.value);
        }

        return ret;
}

