
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
        trace(0, TR_TRACEME, NULL, NULL);

        // Just to be a little more interesting
        // raise(SIGSTOP);

        // return execve(args[0], args, NULL);
        printf("Hello World\n");
        exit(1);
}

void print_frame(interrupt_frame *r) {
        /*
        printf("syscall args:\n");
        printf("  ax: %p\n", r->rax);
        printf("  di: %p\n", r->rdi);
        printf("  si: %p\n", r->rsi);
        printf("  dx: %p\n", r->rdx);
        printf("  cx: %p\n", r->rcx);
        printf("  r8: %p\n", r->r8);
        printf("  r9: %p\n", r->r9);
        */
        /*
        printf("  ds: %#llx\n", r->ds);
        printf("  r15: %#llx\n", r->r15);
        printf("  r14: %#llx\n", r->r14);
        printf("  r13: %#llx\n", r->r13);
        printf("  r12: %#llx\n", r->r12);
        printf("  r11: %#llx\n", r->r11);
        printf("  r10: %#llx\n", r->r10);
        printf("  r9: %#llx\n", r->r9);
        printf("  r8: %#llx\n", r->r8);
        printf("  bp: %#llx\n", r->bp);
        printf("  rdi: %#llx\n", r->rdi);
        printf("  rsi: %#llx\n", r->rsi);
        printf("  rdx: %#llx\n", r->rdx);
        printf("  rbx: %#llx\n", r->rbx);
        printf("  rcx: %#llx\n", r->rcx);
        printf("  rax: %#llx\n", r->rax);
        printf("  interrupt_number: %#llx\n", r->interrupt_number);
        printf("  error_code: %#llx\n", r->error_code);
        printf("  ip: %#llx\n", r->ip);
        printf("  cs: %#llx\n", r->cs);
        printf("  flags: %#llx\n", r->flags);
        printf("  user_sp: %#llx\n", r->user_sp);
        printf("  ss: %#llx\n", r->ss);
        */
#if X86_64
        printf("  ax: %#llx\n", r->rax);
#else
        printf("  ax: %#llx\n", r->eax);
#endif
}

noreturn void fail(const char *str) {
        perror(str);
        exit(1);
}

int main(int argc, char **argv) {
        char **child_args = argv + 1;

        interrupt_frame r;
        int child = exec(NULL);
        int status;

        trace(child, TR_SYSCALL, NULL, NULL);

        while (true) {
                wait(&status);
                if (errno)  fail("wait");
                if (status < 256)  exit(0);

                int event = status &~0xFFFF;
                int syscall = status & 0xFFFF;

                trace(child, TR_GETREGS, NULL, &r);

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


                trace(child, TR_SYSCALL, NULL, NULL);
        }
}

