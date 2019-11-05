
#include <ng/basic.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h> // syscall sys_* prototypes
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/cpu.h>
#include <ng/fs.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>

typedef struct syscall_ret syscall_t();
const uintptr_t syscall_table[] = {
        [NG_DEBUGPRINT] = 0, // removed
        [NG_EXIT]       = (uintptr_t) sys_exit,
        [NG_OPEN]       = (uintptr_t) sys_open,
        [NG_READ]       = (uintptr_t) sys_read,
        [NG_WRITE]      = (uintptr_t) sys_write,
        [NG_FORK]       = (uintptr_t) sys_fork,
        [NG_TOP]        = (uintptr_t) sys_top,
        [NG_GETPID]     = (uintptr_t) sys_getpid,
        [NG_GETTID]     = (uintptr_t) sys_gettid,
        [NG_EXECVE]     = (uintptr_t) sys_execve,
        [NG_WAIT4]      = 0, // removed
        [NG_SOCKET]     = (uintptr_t) sys_socket,
        [NG_BIND0]      = 0, // removed
        [NG_CONNECT0]   = 0, // removed
        [NG_STRACE]     = (uintptr_t) sys_strace,
        [NG_BIND]       = (uintptr_t) sys_bind,
        [NG_CONNECT]    = (uintptr_t) sys_connect,
        [NG_SEND]       = (uintptr_t) sys_send,
        [NG_SENDTO]     = (uintptr_t) sys_sendto,
        [NG_RECV]       = (uintptr_t) sys_recv,
        [NG_RECVFROM]   = (uintptr_t) sys_recvfrom,
        [NG_WAITPID]    = (uintptr_t) sys_waitpid,
        [NG_DUP2]       = (uintptr_t) sys_dup2,
        [NG_UNAME]      = (uintptr_t) sys_uname,
        [NG_YIELD]      = (uintptr_t) sys_yield,
        [NG_SEEK]       = (uintptr_t) sys_seek,
        [NG_POLL]       = (uintptr_t) sys_poll,
        [NG_MMAP]       = (uintptr_t) sys_mmap,
        [NG_MUNMAP]     = (uintptr_t) sys_munmap,
        [NG_HEAPDBG]    = (uintptr_t) sys_heapdbg,
        [NG_SETPGID]    = (uintptr_t) sys_setpgid,
        [NG_EXIT_GROUP] = (uintptr_t) sys_exit_group,
        [NG_CLONE0]     = (uintptr_t) sys_clone0,
        [NG_LOADMOD]    = (uintptr_t) sys_loadmod,
        [NG_HALTVM]     = (uintptr_t) sys_haltvm,
        [NG_OPENAT]     = (uintptr_t) sys_openat,
        [NG_EXECVEAT]   = (uintptr_t) sys_execveat,
        [NG_TTYCTL]     = (uintptr_t) sys_ttyctl,
        [NG_CLOSE]      = (uintptr_t) sys_close,
        [NG_PIPE]       = (uintptr_t) sys_pipe,
        [NG_SIGACTION]  = (uintptr_t) sys_sigaction,
        [NG_SIGRETURN]  = (uintptr_t) sys_sigreturn,
        [NG_KILL]       = (uintptr_t) sys_kill,
};

const char *const syscall_debuginfos[] = {
        [NG_EXIT]       = "exit(%zi)",
        [NG_OPEN]       = "open(\"%s\", %zi)",
        [NG_READ]       = "read(%zi, %p, %zu)",
        [NG_WRITE]      = "write(%zi, %p, %zu)",
        [NG_TOP]        = "top(%zi)",
        [NG_FORK]       = "fork()",
        [NG_GETPID]     = "getpid()",
        [NG_GETTID]     = "gettid()",
        [NG_EXECVE]     = "execve(\"%s\", %p, %p)",
        [NG_WAIT4]      = "wait4(%zi)",
        [NG_SOCKET]     = "socket(%zi, %zi, %zi)",
        [NG_STRACE]     = "strace(%zi)",
        [NG_BIND]       = "bind(%zi, %p, %zu)",
        [NG_CONNECT]    = "connect(%zi, %p, %zu)",
        [NG_SEND]       = "send(%zi, %p, %zu, %zi)",
        [NG_SENDTO]     = "sendto(%zi, %p, %zu, %zi, %p, %zu)",
        [NG_RECV]       = "recv(%zi, %p, %zu, %zi)",
        [NG_RECVFROM]   = "recvfrom(%zi, %p, %zu, %zi, %p, %p)",
        [NG_WAITPID]    = "waitpid(%zi, %p, %#zx)",
        [NG_DUP2]       = "dup2(%zi, %zi)",
        [NG_UNAME]      = "uname(%p)",
        [NG_YIELD]      = "yield()",
        [NG_SEEK]       = "seek(%zi, %zi, %zi)",
        [NG_POLL]       = "poll(%p, %zi, %zi)",
        [NG_MMAP]       = "mmap(%p, %zu, %zi, %zi, %zi, %zi)",
        [NG_MUNMAP]     = "munmap(%p, %zu)",
        [NG_HEAPDBG]    = "heapdbg(%zi)",
        [NG_SETPGID]    = "setpgid()",
        [NG_EXIT_GROUP] = "exit_group(%zi)",
        [NG_CLONE0]     = "clone0(%p, %p, %p, %zi)",
        [NG_LOADMOD]    = "loadmod(%zi)",
        [NG_HALTVM]     = "haltvm(%zi)",
        [NG_EXECVEAT]   = "execveat(%zi, %s, %p, %p)",
        [NG_TTYCTL]     = "ttyctl(%zi, %zi, %zi)",
        [NG_CLOSE]      = "close(%zi)",
        [NG_PIPE]       = "pipe(%p)",
        [NG_SIGACTION]  = "sigaction(%zi, %p, %zi)",
        [NG_SIGRETURN]  = "sigreturn(%zi)",
        [NG_KILL]       = "kill(%zi, %zi)",
};

const unsigned int syscall_ptr_mask[] = {
        [NG_EXIT]       = 0,
        [NG_OPEN]       = 0x01,
        [NG_READ]       = 0x02,
        [NG_WRITE]      = 0x02,
        [NG_FORK]       = 0,
        [NG_TOP]        = 0,
        [NG_GETPID]     = 0,
        [NG_GETTID]     = 0,
        [NG_EXECVE]     = 0x06,
        [NG_WAIT4]      = 0,
        [NG_SOCKET]     = 0,
        [NG_STRACE]     = 0,
        [NG_BIND]       = 0x02,
        [NG_CONNECT]    = 0x02,
        [NG_SEND]       = 0x02,
        [NG_SENDTO]     = 0x12,
        [NG_RECV]       = 0x02,
        [NG_RECVFROM]   = 0x32,
        [NG_WAITPID]    = 0x02,
        [NG_DUP2]       = 0,
        [NG_UNAME]      = 0x01,
        [NG_YIELD]      = 0,
        [NG_SEEK]       = 0,
        [NG_POLL]       = 0x01,
        [NG_MMAP]       = 0x01,
        [NG_MUNMAP]     = 0x01,
        [NG_HEAPDBG]    = 0,
        [NG_SETPGID]    = 0,
        [NG_EXIT_GROUP] = 0,
        [NG_CLONE0]     = 0x07,
        [NG_LOADMOD]    = 0,
        [NG_HALTVM]     = 0,
        [NG_OPENAT]     = 0x02,
        [NG_EXECVEAT]   = 0x0C,
        [NG_TTYCTL]     = 0,
        [NG_CLOSE]      = 0,
        [NG_PIPE]       = 0x01,
        [NG_SIGACTION]  = 0x02,
        [NG_SIGRETURN]  = 0,
        [NG_KILL]       = 0,
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

        if (syscall_num >= SYSCALL_MAX || syscall_num <= NG_INVALID) {
                panic("invalid syscall number: %i\n", syscall_num);
        }

        unsigned int mask = syscall_ptr_mask[syscall_num];
        check_ptr(mask & 0x01, arg1);
        check_ptr(mask & 0x02, arg2);
        check_ptr(mask & 0x04, arg3);
        check_ptr(mask & 0x08, arg4);
        check_ptr(mask & 0x10, arg5);
        check_ptr(mask & 0x20, arg6);

        if (running_thread->thread_flags & THREAD_STRACE) {
                printf(syscall_debuginfos[syscall_num],
                        arg1, arg2, arg3, arg4, arg5, arg6);
        }

        syscall_t *const call = (syscall_t *const)syscall_table[syscall_num];
        struct syscall_ret ret = {0};

        if (call == 0) {
                ret.error = EINVAL;
        } else {
                if (syscall_num == NG_EXECVE || syscall_num == NG_FORK ||
                    syscall_num == NG_CLONE0) {
                        ret = call(frame, arg1, arg2, arg3, arg4, arg5, arg6);
                } else {
                        ret = call(arg1, arg2, arg3, arg4, arg5, arg6);
                }
        }

        if (running_thread->thread_flags & THREAD_STRACE) {
                printf(" -> %s(%lu)\n", ret.error ? "error" : "value",
                       ret.error ? ret.error : ret.value);
        }

        return ret;
}

