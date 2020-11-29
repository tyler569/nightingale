// vim: ts=4 sw=4 sts=4 :

#include "sh.h"
#include "parse.h"
#include "readline.h"
#include "token.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ttyctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector.h>

bool do_buffer = true;
bool do_token_debug = false;
bool interactive = true;

int exec(char **argv) {
    pid_t child;

    child = fork();
    if (child == -1) {
        perror("fork()");
        return -1;
    }
    if (child == 0) {
        int pid = getpid();
        setpgid(pid, pid);

        ttyctl(STDIN_FILENO, TTY_SETBUFFER, 1);
        ttyctl(STDIN_FILENO, TTY_SETECHO, 1);
        ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());

        execve(argv[0], argv, NULL);

        // getting here constitutes failure
        switch (errno) {
        case ENOENT: printf("%s does not exist\n", argv[0]); break;
        case ENOEXEC:
            printf("%s is not executable or is not a valid format\n", argv[0]);
            break;
        default:
            printf("An unknown error occured running %s\n", argv[0]);
            perror("execve()");
        }

        exit(127);
    } else {
        int return_code;
        if (waitpid(child, &return_code, 0) == -1) {
            perror("waitpid()");
            return -1;
        }
        return return_code;
    }
}

int run(struct sh_command *cmd) {
    while (cmd) {
        pid_t child = fork();
        if (child == 0) {
            if (cmd->input) { dup2(cmd->input, STDIN_FILENO); }
            if (cmd->output) { dup2(cmd->output, STDOUT_FILENO); }

            int pid = getpid();
            setpgid(pid, pid);

            ttyctl(STDIN_FILENO, TTY_SETBUFFER, 1);
            ttyctl(STDIN_FILENO, TTY_SETECHO, 1);
            ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());

            char **argv = cmd->args;

            execve(argv[0], argv, NULL);

            if (errno == ENOENT) {
                printf("%s: command not found\n", argv[0]);
                exit(127);
            }
            perror("execve");
            exit(126);
        } else {
            if (cmd->input) { close(cmd->input); }
            if (cmd->output) { close(cmd->output); }
        }
        cmd = cmd->next;
    }

    int return_code;
    while (errno != ECHILD) {
        int c = waitpid(-1, &return_code, 0);
        if (c == -1 && errno != ECHILD) {
            perror("waitpid()");
            return -1;
        }
    }
    errno = 0;
    return return_code;
}


int handle_one_line() {
    if (interactive) {
        printf("$ ");
        fflush(stdout);
    }

    char cmdline[256] = {0};

    if (do_buffer && interactive) {
        ttyctl(STDIN_FILENO, TTY_SETBUFFER, 0);
        ttyctl(STDIN_FILENO, TTY_SETECHO, 0);
        ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());
        if (read_line_interactive(cmdline, 256) == -1) { return 2; }
    } else {
        ttyctl(STDIN_FILENO, TTY_SETBUFFER, 1);
        ttyctl(STDIN_FILENO, TTY_SETECHO, 1);
        ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());
        if (read_line_simple(cmdline, 256) == -1) { return 2; }
    }

    if (cmdline[0] == 0) return 0;

    struct vector *tokens = tokenize_string(cmdline);

    if (do_token_debug) print_token_vector(tokens);

    struct sh_command *instruction = parse_line(tokens, 0, 0);
    free_token_vector(tokens);

    if (!instruction) {
        printf("parse error\n");
        return 0;
    }

    char *arg_0 = instruction->arg_buf;
    if (arg_0[0] == 0) return 0;
    if (strncmp("exit", arg_0, 4) == 0) return 1;
    int ret_val = run(instruction);
    recursive_free_sh_command(instruction);

    if (ret_val >= 128 && ret_val < 128 + 32) {
        // TODO: signal names
        printf("terminated by signal %i\n", ret_val - 128);
    } else if (ret_val != 0) {
        printf("-> %i\n", ret_val);
    }

    return 0;
}

void signal_handler(int signal) {
    // TODO stop reading line, dump history, start new line
    printf("\n");
}

void help(const char *progname) {
    fprintf(stderr,
            "usage: %s [-nd]\n"
            "  -n     disable tty buffering\n"
            "  -d     token debug mode\n",
            progname);
}

int main(int argc, char **argv) {
    int pid = getpid();
    setpgid(pid, pid);

    signal(SIGINT, signal_handler);

    int opt;
    while ((opt = getopt(argc, argv, "ndh")) != -1) {
        switch (opt) {
        case 'n': do_buffer = false; break;
        case 'd': do_token_debug = true; break;
        case '?': // FALLTHROUGH
        case 'h': help(argv[0]); return 0;
        }
    }

    if (argv[optind]) {
        FILE *file = fopen(argv[optind], "r");
        if (file) {
            dup2(0, fileno(file));
            do_buffer = false;
            interactive = false;
        }
    }
    if (!isatty(fileno(stdin))) interactive = false;
    if (interactive) printf("Nightingale shell\n");

    while (handle_one_line() == 0) {}

    return 0;
}
