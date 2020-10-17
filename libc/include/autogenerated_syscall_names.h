const char *syscall_names[] = {
    [NG_EXIT] = "exit",
    [NG_OPEN] = "open",
    [NG_READ] = "read",
    [NG_WRITE] = "write",
    [NG_FORK] = "fork",
    [NG_TOP] = "top",
    [NG_GETPID] = "getpid",
    [NG_GETTID] = "gettid",
    [NG_EXECVE] = "execve",
    [NG_STRACE] = "strace",
    [NG_WAITPID] = "waitpid",
    [NG_DUP2] = "dup2",
    [NG_UNAME] = "uname",
    [NG_YIELD] = "yield",
    [NG_SEEK] = "seek",
    [NG_POLL] = "poll",
    [NG_MMAP] = "mmap",
    [NG_MUNMAP] = "munmap",
    [NG_SETPGID] = "setpgid",
    [NG_EXIT_GROUP] = "exit_group",
    [NG_CLONE0] = "clone0",
    [NG_LOADMOD] = "loadmod",
    [NG_HALTVM] = "haltvm",
    [NG_OPENAT] = "openat",
    [NG_EXECVEAT] = "execveat",
    [NG_TTYCTL] = "ttyctl",
    [NG_CLOSE] = "close",
    [NG_PIPE] = "pipe",
    [NG_SIGACTION] = "sigaction",
    [NG_SIGRETURN] = "sigreturn",
    [NG_KILL] = "kill",
    [NG_SLEEPMS] = "sleepms",
    [NG_GETDIRENTS] = "getdirents",
    [NG_XTIME] = "xtime",
    [NG_CREATE] = "create",
    [NG_PROCSTATE] = "procstate",
    [NG_FAULT] = "fault",
    [NG_TRACE] = "trace",
    [NG_SIGPROCMASK] = "sigprocmask",
};
