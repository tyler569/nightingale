
#include <basic.h>
#include <ng/panic.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h> // syscall sys_* prototypes
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/cpu.h>
#include <ng/fs.h>
#include <ng/syscall_consts.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef sysret (*syscall_fptr_t)();
syscall_fptr_t syscall_table[128] = {
        [NG_DEBUGPRINT]  = 0, // removed
        [NG_EXIT]        = sys_exit,
        [NG_OPEN]        = sys_open,
        [NG_READ]        = sys_read,
        [NG_WRITE]       = sys_write,
        [NG_FORK]        = sys_fork,
        [NG_TOP]         = sys_top,
        [NG_GETPID]      = sys_getpid,
        [NG_GETTID]      = sys_gettid,
        [NG_EXECVE]      = sys_execve,
        [NG_WAIT4]       = 0, // removed
        [NG_SOCKET]      = 0, // sys_socket,
        [NG_BIND0]       = 0, // removed
        [NG_CONNECT0]    = 0, // removed
        [NG_STRACE]      = sys_strace,
        [NG_BIND]        = 0, // sys_bind,
        [NG_CONNECT]     = 0, // sys_connect,
        [NG_SEND]        = 0, // sys_send,
        [NG_SENDTO]      = 0, // sys_sendto,
        [NG_RECV]        = 0, // sys_recv,
        [NG_RECVFROM]    = 0, // sys_recvfrom,
        [NG_WAITPID]     = sys_waitpid,
        [NG_DUP2]        = sys_dup2,
        [NG_UNAME]       = sys_uname,
        [NG_YIELD]       = sys_yield,
        [NG_SEEK]        = sys_seek,
        [NG_POLL]        = sys_poll,
        [NG_MMAP]        = sys_mmap,
        [NG_MUNMAP]      = sys_munmap,
        [NG_HEAPDBG]     = 0, // removed
        [NG_SETPGID]     = sys_setpgid,
        [NG_EXIT_GROUP]  = sys_exit_group,
        [NG_CLONE0]      = sys_clone0,
        [NG_LOADMOD]     = sys_loadmod,
        [NG_HALTVM]      = sys_haltvm,
        [NG_OPENAT]      = sys_openat,
        [NG_EXECVEAT]    = sys_execveat,
        [NG_TTYCTL]      = sys_ttyctl,
        [NG_CLOSE]       = sys_close,
        [NG_PIPE]        = sys_pipe,
        [NG_SIGACTION]   = sys_sigaction,
        [NG_SIGRETURN]   = sys_sigreturn,
        [NG_KILL]        = sys_kill,
        [NG_SLEEPMS]     = sys_sleepms,
        [NG_GETDIRENTS]  = sys_getdirents,
        [NG_TIME]        = sys_time,
        [NG_CREATE]      = sys_create,
        [NG_PROCSTATE]   = sys_procstate,
        [NG_FAULT]       = sys_fault,
        [NG_TRACE]       = sys_trace,
        [NG_SIGPROCMASK] = sys_sigprocmask,
};

const char *const syscall_debuginfos[] = {
        [NG_EXIT]        = "exit(%zi)",
        [NG_OPEN]        = "open(\"%s\", %zi, %zo)",
        [NG_READ]        = "read(%zi, %p, %zu)",
        [NG_WRITE]       = "write(%zi, %p, %zu)",
        [NG_TOP]         = "top(%zi)",
        [NG_FORK]        = "fork()",
        [NG_GETPID]      = "getpid()",
        [NG_GETTID]      = "gettid()",
        [NG_EXECVE]      = "execve(\"%s\", %p, %p)",
        [NG_WAIT4]       = "wait4(%zi)",
        [NG_SOCKET]      = "socket(%zi, %zi, %zi)",
        [NG_STRACE]      = "strace(%zi)",
        [NG_BIND]        = "bind(%zi, %p, %zu)",
        [NG_CONNECT]     = "connect(%zi, %p, %zu)",
        [NG_SEND]        = "send(%zi, %p, %zu, %zi)",
        [NG_SENDTO]      = "sendto(%zi, %p, %zu, %zi, %p, %zu)",
        [NG_RECV]        = "recv(%zi, %p, %zu, %zi)",
        [NG_RECVFROM]    = "recvfrom(%zi, %p, %zu, %zi, %p, %p)",
        [NG_WAITPID]     = "waitpid(%zi, %p, %#zx)",
        [NG_DUP2]        = "dup2(%zi, %zi)",
        [NG_UNAME]       = "uname(%p)",
        [NG_YIELD]       = "yield()",
        [NG_SEEK]        = "seek(%zi, %zi, %zi)",
        [NG_POLL]        = "poll(%p, %zi, %zi)",
        [NG_MMAP]        = "mmap(%p, %zu, %zi, %zi, %zi, %zi)",
        [NG_MUNMAP]      = "munmap(%p, %zu)",
        [NG_HEAPDBG]     = "heapdbg(%zi)",
        [NG_SETPGID]     = "setpgid(%zi, %zi)",
        [NG_EXIT_GROUP]  = "exit_group(%zi)",
        [NG_CLONE0]      = "clone0(%p, %p, %p, %zi)",
        [NG_LOADMOD]     = "loadmod(%zi)",
        [NG_HALTVM]      = "haltvm(%zi)",
        [NG_EXECVEAT]    = "execveat(%zi, %s, %p, %p)",
        [NG_TTYCTL]      = "ttyctl(%zi, %zi, %zi)",
        [NG_CLOSE]       = "close(%zi)",
        [NG_PIPE]        = "pipe(%p)",
        [NG_SIGACTION]   = "sigaction(%zi, %p, %zi)",
        [NG_SIGRETURN]   = "sigreturn(%zi)",
        [NG_KILL]        = "kill(%zi, %zi)",
        [NG_SLEEPMS]     = "sleep(%zi)",
        [NG_GETDIRENTS]  = "getdirents(%zi, %p, %zi)",
        [NG_TIME]        = "time()",
        [NG_CREATE]      = "create(\"%s\")",
        [NG_PROCSTATE]   = "procstate(%zi, %zx)",
        [NG_FAULT]       = "fault(%zi)",
        [NG_TRACE]       = "trace(%zi, %zi, %p, %p)",
        [NG_SIGPROCMASK] = "sigprocmask(%zi, %p, %p)",
};

const unsigned int syscall_ptr_mask[] = {
        [NG_EXIT]        = 0,
        [NG_OPEN]        = 0x01,
        [NG_READ]        = 0x02,
        [NG_WRITE]       = 0x02,
        [NG_FORK]        = 0,
        [NG_TOP]         = 0,
        [NG_GETPID]      = 0,
        [NG_GETTID]      = 0,
        [NG_EXECVE]      = 0x06,
        [NG_WAIT4]       = 0,
        [NG_SOCKET]      = 0,
        [NG_STRACE]      = 0,
        [NG_BIND]        = 0x02,
        [NG_CONNECT]     = 0x02,
        [NG_SEND]        = 0x02,
        [NG_SENDTO]      = 0x12,
        [NG_RECV]        = 0x02,
        [NG_RECVFROM]    = 0x32,
        [NG_WAITPID]     = 0x02,
        [NG_DUP2]        = 0,
        [NG_UNAME]       = 0x01,
        [NG_YIELD]       = 0,
        [NG_SEEK]        = 0,
        [NG_POLL]        = 0x01,
        [NG_MMAP]        = 0x01,
        [NG_MUNMAP]      = 0x01,
        [NG_HEAPDBG]     = 0,
        [NG_SETPGID]     = 0,
        [NG_EXIT_GROUP]  = 0,
        [NG_CLONE0]      = 0x07,
        [NG_LOADMOD]     = 0,
        [NG_HALTVM]      = 0,
        [NG_OPENAT]      = 0x02,
        [NG_EXECVEAT]    = 0x0C,
        [NG_TTYCTL]      = 0,
        [NG_CLOSE]       = 0,
        [NG_PIPE]        = 0x01,
        [NG_SIGACTION]   = 0x02,
        [NG_SIGRETURN]   = 0,
        [NG_KILL]        = 0,
        [NG_SLEEPMS]     = 0,
        [NG_GETDIRENTS]  = 0x02,
        [NG_TIME]        = 0,
        [NG_CREATE]      = 0x01,
        [NG_PROCSTATE]   = 0,
        [NG_FAULT]       = 0,
        [NG_TRACE]       = 0, // plenty of trace calls put non-pointers in args 3 and 4
        [NG_SIGPROCMASK] = 0x06,
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
                if (running_thread->flags & TF_SYSCALL_TRACE) \
                        printf(" -> <EFAULT>\n"); \
                return -EFAULT; \
        }

void syscall_entry(interrupt_frame *r, int syscall) {
        if (running_thread->tracer) {
                trace_syscall_entry(running_thread, syscall);
        }
}

void syscall_exit(interrupt_frame *r, int syscall) {
        if (running_thread->tracer) {
                trace_syscall_exit(running_thread, syscall);
        }
}

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
sysret do_syscall_with_table(enum ng_syscall syscall_num, intptr_t arg1,
                intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5,
                intptr_t arg6, interrupt_frame *frame) {

        if (syscall_num >= SYSCALL_MAX || syscall_num <= NG_INVALID) {
                panic("invalid syscall number: %i\n", syscall_num);
        }

        if (running_thread->flags & TF_SYSCALL_TRACE) {
                printf("[%i:%i] ", running_process->pid, running_thread->tid);
                printf(syscall_debuginfos[syscall_num],
                       arg1, arg2, arg3, arg4, arg5, arg6);
        }

        unsigned int mask = syscall_ptr_mask[syscall_num];
        check_ptr(mask & 0x01, arg1);
        check_ptr(mask & 0x02, arg2);
        check_ptr(mask & 0x04, arg3);
        check_ptr(mask & 0x08, arg4);
        check_ptr(mask & 0x10, arg5);
        check_ptr(mask & 0x20, arg6);

        syscall_fptr_t call = syscall_table[syscall_num];
        sysret ret = {0};

        if (call == 0) {
                ret = -EINVAL;
        } else {
                if (syscall_num == NG_EXECVE ||
                    syscall_num == NG_FORK ||
                    syscall_num == NG_CLONE0) {
                        ret = call(frame, arg1, arg2, arg3, arg4, arg5, arg6);
                } else {
                        ret = call(arg1, arg2, arg3, arg4, arg5, arg6);
                }
        }

        if (running_thread->flags & TF_SYSCALL_TRACE) {
                if (syscall_num == NG_STRACE) {
                        // This is just here to mark this as a strace return,
                        // since it can be confusing that " -> 0" appears
                        // after some other random syscall when the strace
                        // call returns.
                        printf("XX");
                }
                if (ret >= 0 && ret < 0x100000) {
                        printf(" -> %lu\n", ret);
                } else if (ret >= 0 || ret < -0x1000) {
                        printf(" -> %#lx\n", ret);
                } else {
                        printf(" -> <%s>\n", errno_names[-ret]);
                }
        }

        return ret;
}

