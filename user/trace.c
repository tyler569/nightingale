
#include <stdio.h>
#include <stdlib.h>
#include <sys/trace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>

#include <ng/syscall_consts.h> // syscall numbers
#include <ng/x86/cpu.h> // interrupt_frame

const char *syscall_names[] = {
        [NG_INVALID] =     "invalid",
        [NG_DEBUGPRINT] =  "debugprint",
        [NG_EXIT] =        "exit",
        [NG_OPEN] =        "open",
        [NG_READ] =        "read",
        [NG_WRITE] =       "write",
        [NG_FORK] =        "fork",
        [NG_TOP] =         "top",
        [NG_GETPID] =      "getpid",
        [NG_GETTID] =      "gettid",
        [NG_EXECVE] =      "execve",
        [NG_WAIT4] =       "wait4",
        [NG_SOCKET] =      "socket",
        [NG_BIND0] =       "bind0",
        [NG_CONNECT0] =    "connect0",
        [NG_STRACE] =      "strace",
        [NG_BIND] =        "bind",
        [NG_CONNECT] =     "connect",
        [NG_SEND] =        "send",
        [NG_SENDTO] =      "sendto",
        [NG_RECV] =        "recv",
        [NG_RECVFROM] =    "recvfrom",
        [NG_WAITPID] =     "waitpid",
        [NG_DUP2] =        "dup2",
        [NG_UNAME] =       "uname",
        [NG_YIELD] =       "yield",
        [NG_SEEK] =        "seek",
        [NG_POLL] =        "poll",
        [NG_MMAP] =        "mmap",
        [NG_MUNMAP] =      "munmap",
        [NG_HEAPDBG] =     "heapdbg",
        [NG_SETPGID] =     "setpgid",
        [NG_EXIT_GROUP] =  "exit_group",
        [NG_CLONE0] =      "clone0",
        [NG_LOADMOD] =     "loadmod",
        [NG_HALTVM] =      "haltvm",
        [NG_OPENAT] =      "openat",
        [NG_EXECVEAT] =    "execveat",
        [NG_TTYCTL] =      "ttyctl",
        [NG_CLOSE] =       "close",
        [NG_PIPE] =        "pipe",
        [NG_SIGACTION] =   "sigaction",
        [NG_SIGRETURN] =   "sigreturn",
        [NG_KILL] =        "kill",
        [NG_SLEEPMS] =     "sleepms",
        [NG_GETDIRENTS] =  "getdirents",
        [NG_TIME] =        "time",
        [NG_CREATE] =      "create",
        [NG_PROCSTATE] =   "procstate",
        [NG_FAULT] =       "fault",
        [NG_TRACE] =       "trace",
        [NG_SIGPROCMASK] = "sigprocmask",
};

int exec(char **args) {
        int child = fork();
        if (child)  return child;

        // strace(1);
        trace(TR_TRACEME, 0, NULL, NULL);
        raise(SIGSTOP);

        return execve(args[0], args, NULL);
}

noreturn void fail(const char *str) {
        perror(str);
        exit(1);
}

int main(int argc, char **argv) {
        char **child_args = argv + 1;

        interrupt_frame r;
        int child = exec(argv + 1);
        int status;

        wait(&status);
        trace(TR_SYSCALL, child, NULL, NULL);

        while (true) {
                wait(&status);
                if (errno)  fail("wait");
                if (status < 256)  exit(0);

                int event = status &~0xFFFF;
                int syscall = status & 0xFFFF;

                trace(TR_GETREGS, child, NULL, &r);

                if (event == TRACE_SYSCALL_ENTRY) {
                        printf("syscall_enter: %s\n", syscall_names[syscall]);
                }

                if (event == TRACE_SYSCALL_EXIT) {
                        printf("syscall_exit: %s", syscall_names[syscall]);
#if X86_64
                        printf(" -> %i\n", r.rax);
#elif I686
                        printf(" -> %i\n", r.eax);
#endif
                }

                if (event == TRACE_SIGNAL) {
                        printf("signal: %i\n", syscall);
                }


                trace(TR_SYSCALL, child, NULL, NULL);
        }
}

