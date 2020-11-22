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
    trace(TR_SYSCALL, child, NULL, NULL);

    while (true) {
        wait(&status);
        if (errno) fail("wait");
        if (status < 256) exit(0);

        int event = status & ~0xFFFF;
        int syscall = status & 0xFFFF;

        trace(TR_GETREGS, child, NULL, &r);

        if (event == TRACE_SYSCALL_ENTRY) {
            printf("syscall_enter: %s\n", syscall_names[syscall]);
        }

        if (event == TRACE_SYSCALL_EXIT) {
            printf("syscall_exit: %s", syscall_names[syscall]);
            printf(" -> %zu\n", frame_get(&r, RET_VAL));
        }

        if (event == TRACE_SIGNAL) {
            printf("signal: %i\n", syscall);
            trace(TR_SYSCALL, child, NULL, (void *)(intptr_t)syscall);
        } else {
            trace(TR_SYSCALL, child, NULL, NULL);
        }
    }
}
