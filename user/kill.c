#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *signal_shortnames[] = {
    [SIGABRT] = "ABRT",   [SIGALRM] = "ALRM",     [SIGBUS] = "BUS",
    [SIGCHLD] = "CHLD",   [SIGCONT] = "CONT",     [SIGFPE] = "FPE",
    [SIGHUP] = "HUP",     [SIGILL] = "ILL",       [SIGINFO] = "INFO",
    [SIGINT] = "INT",     [SIGKILL] = "KILL",     [SIGPIPE] = "PIPE",
    [SIGPROF] = "PROF",   [SIGQUIT] = "QUIT",     [SIGSEGV] = "SEGV",
    [SIGSTOP] = "STOP",   [SIGTSTP] = "TSTP",     [SIGSYS] = "SYS",
    [SIGTERM] = "TERM",   [SIGTRAP] = "TRAP",     [SIGTTIN] = "TTIN",
    [SIGTTOU] = "TTOU",   [SIGURG] = "URG",       [SIGUSR1] = "USR1",
    [SIGUSR2] = "USR2",   [SIGVTALRM] = "VTALRM", [SIGXCPU] = "XCPU",
    [SIGWINCH] = "WINCH",
};

int signal_by_name(char *name) {
    int signal = -1;
    for (int i = 0; i <= SIGWINCH; i++) {
        if (strcmp(signal_shortnames[i], name) == 0) {
            signal = i;
            break;
        }
    }
    return signal;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "kill: not enough arguments\n");
        return EXIT_FAILURE;
    }

    int signal = SIGINT;

    if (argv[1][0] == '-') {
        signal = signal_by_name(&argv[1][1]);
        if (signal == -1) {
            signal = atoi(&argv[1][1]);
        }
        if (signal == -1) {
            fprintf(stderr, "%s is not a valid signal\n", &argv[1][1]);
            return EXIT_FAILURE;
        }
        argv += 1;
    }

    if (!argv[1]) {
        fprintf(stderr, "kill: not enough arguments\n");
        return EXIT_FAILURE;
    }

    pid_t pid = atoi(argv[1]);
    int err = kill(pid, signal);
    if (err) {
        perror("kill");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
