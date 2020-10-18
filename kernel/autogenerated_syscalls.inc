sysret (*syscall_table[128])() = {
    [NG_EXIT] = sys_exit,
    [NG_OPEN] = sys_open,
    [NG_READ] = sys_read,
    [NG_WRITE] = sys_write,
    [NG_FORK] = sys_fork,
    [NG_TOP] = sys_top,
    [NG_GETPID] = sys_getpid,
    [NG_GETTID] = sys_gettid,
    [NG_EXECVE] = sys_execve,
    [NG_STRACE] = sys_strace,
    [NG_WAITPID] = sys_waitpid,
    [NG_DUP2] = sys_dup2,
    [NG_UNAME] = sys_uname,
    [NG_YIELD] = sys_yield,
    [NG_SEEK] = sys_seek,
    [NG_POLL] = sys_poll,
    [NG_MMAP] = sys_mmap,
    [NG_MUNMAP] = sys_munmap,
    [NG_SETPGID] = sys_setpgid,
    [NG_EXIT_GROUP] = sys_exit_group,
    [NG_CLONE0] = sys_clone0,
    [NG_LOADMOD] = sys_loadmod,
    [NG_HALTVM] = sys_haltvm,
    [NG_OPENAT] = sys_openat,
    [NG_EXECVEAT] = sys_execveat,
    [NG_TTYCTL] = sys_ttyctl,
    [NG_CLOSE] = sys_close,
    [NG_PIPE] = sys_pipe,
    [NG_SIGACTION] = sys_sigaction,
    [NG_SIGRETURN] = sys_sigreturn,
    [NG_KILL] = sys_kill,
    [NG_SLEEPMS] = sys_sleepms,
    [NG_GETDIRENTS] = sys_getdirents,
    [NG_XTIME] = sys_xtime,
    [NG_CREATE] = sys_create,
    [NG_PROCSTATE] = sys_procstate,
    [NG_FAULT] = sys_fault,
    [NG_TRACE] = sys_trace,
    [NG_SIGPROCMASK] = sys_sigprocmask,
    [NG_SYSCALL_TEST] = sys_syscall_test,
};
const char *const syscall_debuginfos[128] = {
    [NG_EXIT] = "exit(%zi)",
    [NG_OPEN] = "open(\"%s\", %zi, %zi)",
    [NG_READ] = "read(%zi, %p, %zu)",
    [NG_WRITE] = "write(%zi, %p, %zu)",
    [NG_FORK] = "fork()",
    [NG_TOP] = "top(%zi)",
    [NG_GETPID] = "getpid()",
    [NG_GETTID] = "gettid()",
    [NG_EXECVE] = "execve(\"%s\", %p, %p)",
    [NG_STRACE] = "strace(%zi)",
    [NG_WAITPID] = "waitpid(%zi, %p, %zi)",
    [NG_DUP2] = "dup2(%zi, %zi)",
    [NG_UNAME] = "uname(%p)",
    [NG_YIELD] = "yield()",
    [NG_SEEK] = "seek(%zi, %zi, %zi)",
    [NG_POLL] = "poll(%p, %zu, %zi)",
    [NG_MMAP] = "mmap(%p, %zu, %zi, %zi, %zi, %zi)",
    [NG_MUNMAP] = "munmap(%p, %zu)",
    [NG_SETPGID] = "setpgid(%zi, %zi)",
    [NG_EXIT_GROUP] = "exit_group(%zi)",
    [NG_CLONE0] = "clone0(%p, %p, %p, %zi)",
    [NG_LOADMOD] = "loadmod(%zi)",
    [NG_HALTVM] = "haltvm(%zi)",
    [NG_OPENAT] = "openat(%zi, \"%s\", %zi)",
    [NG_EXECVEAT] = "execveat(%zi, \"%s\", %p, %p)",
    [NG_TTYCTL] = "ttyctl(%zi, %zi, %zi)",
    [NG_CLOSE] = "close(%zi)",
    [NG_PIPE] = "pipe(%p)",
    [NG_SIGACTION] = "sigaction(%zi, %p, %zi)",
    [NG_SIGRETURN] = "sigreturn(%zi)",
    [NG_KILL] = "kill(%zi, %zi)",
    [NG_SLEEPMS] = "sleepms(%zi)",
    [NG_GETDIRENTS] = "getdirents(%zi, %p, %zi)",
    [NG_XTIME] = "xtime()",
    [NG_CREATE] = "create(\"%s\")",
    [NG_PROCSTATE] = "procstate(%zi, %zi)",
    [NG_FAULT] = "fault(%zi)",
    [NG_TRACE] = "trace(%zi, %zi, %p, %p)",
    [NG_SIGPROCMASK] = "sigprocmask(%zi, %p, %p)",
    [NG_SYSCALL_TEST] = "syscall_test(\"%s\")",
};
const unsigned int syscall_ptr_mask[128] = {
    [NG_EXIT] = 0,
    [NG_OPEN] = 1,
    [NG_READ] = 2,
    [NG_WRITE] = 2,
    [NG_FORK] = 0,
    [NG_TOP] = 0,
    [NG_GETPID] = 0,
    [NG_GETTID] = 0,
    [NG_EXECVE] = 7,
    [NG_STRACE] = 0,
    [NG_WAITPID] = 2,
    [NG_DUP2] = 0,
    [NG_UNAME] = 1,
    [NG_YIELD] = 0,
    [NG_SEEK] = 0,
    [NG_POLL] = 1,
    [NG_MMAP] = 1,
    [NG_MUNMAP] = 1,
    [NG_SETPGID] = 0,
    [NG_EXIT_GROUP] = 0,
    [NG_CLONE0] = 7,
    [NG_LOADMOD] = 0,
    [NG_HALTVM] = 0,
    [NG_OPENAT] = 2,
    [NG_EXECVEAT] = 14,
    [NG_TTYCTL] = 0,
    [NG_CLOSE] = 0,
    [NG_PIPE] = 1,
    [NG_SIGACTION] = 0,
    [NG_SIGRETURN] = 0,
    [NG_KILL] = 0,
    [NG_SLEEPMS] = 0,
    [NG_GETDIRENTS] = 2,
    [NG_XTIME] = 0,
    [NG_CREATE] = 1,
    [NG_PROCSTATE] = 0,
    [NG_FAULT] = 0,
    [NG_TRACE] = 0,
    [NG_SIGPROCMASK] = 6,
    [NG_SYSCALL_TEST] = 1,
};