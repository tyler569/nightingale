#include <errno.h>
#include <nightingale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/trace.h>
#include <sys/wait.h>
#include <unistd.h>

int exec(char **args) {
    int child = fork();
    if (child) return child;

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
    if (!argv[1]) {
        fprintf(stderr, "No command specified\n");
        exit(EXIT_FAILURE);
    }

    char **child_args = argv + 1;

    interrupt_frame r;
    int child = exec(argv + 1);
    int status;

    wait(&status);
    trace(TR_SINGLESTEP, child, NULL, NULL);

    while (true) {
        wait(&status);
        if (errno) fail("wait");
        if (status < 256) exit(0);

        int event = status & ~0xFFFF;
        int syscall = status & 0xFFFF;
        intptr_t signal = 0;

        trace(TR_GETREGS, child, NULL, &r);

        if (event == TRACE_SYSCALL_ENTRY) {
            printf("syscall_enter: %s\n", syscall_names[syscall]);
        }

        if (event == TRACE_SYSCALL_EXIT) {
            printf("syscall_exit: %s", syscall_names[syscall]);
            printf(" -> %zu\n", FRAME_RETURN(&r));
        }

        if (event == TRACE_SIGNAL) {
            printf("signal: %i\n", syscall);
            signal = syscall;
        }

        if (event == TRACE_TRAP) printf("step: %#10zx\n", r.ip);

        trace(TR_SINGLESTEP, child, NULL, (void *)signal);
    }
}
