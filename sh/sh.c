// vim: ts=4 sw=4 sts=4 :

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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ttyctl.h>
#include <vector.h>
#include <stdbool.h>
#include "sh.h"
#include "token.h"
#include "readline.h"
#include "parse.h"

bool do_buffer = true;
bool do_token_debug = false;
bool interactive;

int exec(char *const *argv) {
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
            case ENOENT:
                printf("%s does not exist\n", argv[0]);
                break;
            case ENOEXEC:
                printf("%s is not executable or is not a valid format\n",
                        argv[0]);
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
            if (cmd->input)
                dup2(cmd->input, STDIN_FILENO);
            if (cmd->output)
                dup2(cmd->output, STDOUT_FILENO);

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
        } 
        else {
            if (cmd->input)
                close(cmd->input);
            if (cmd->output)
                close(cmd->output);

            // int return_code;
            // int c = waitpid(child, &return_code, 0);
            // if (c < 0) {
            //     perror("waitpid()");
            //     return -1;
            // }
            // return return_code;
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
    // printf("r");
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
        if (read_line_interactive(cmdline, 256) == -1) {
            return 2;
        }
    } else {
        ttyctl(STDIN_FILENO, TTY_SETBUFFER, 1);
        ttyctl(STDIN_FILENO, TTY_SETECHO, 1);
        ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());
        if (read_line_simple(cmdline, 256) == -1) {
            return 2;
        }
    }

    if (cmdline[0] == 0)
        return 0;

    struct vector *tokens = tokenize_string(cmdline);

    if (do_token_debug)
        print_token_vector(tokens);

    struct sh_command *instruction = parse_line(tokens, 0, 0);
    free_token_vector(tokens);

    if (!instruction) {
        printf("parse error\n");
        return 0;
    }

    char *arg_0 = instruction->arg_buf;

    if (arg_0[0] == 0) {
        return 0;
    }

    if (strncmp("exit", arg_0, 4) == 0) {
        return 1;
    }

    int ret_val = run(instruction);

    recursive_free_sh_command(instruction);

    if (ret_val > 256 && ret_val < 300) {
        printf("terminated by signal %i\n", ret_val - 256);
    } else if (ret_val != 0) {
        printf("-> %i\n", ret_val);
    }

    return 0;
}

void signal_handler(int signal) {
    printf("\n");
}

typedef void (*option_action)(void);
struct option_action_entry {
    const char *name;
    option_action action;
};

void action_set_nobuffer() {
    do_buffer = false;
    printf("No buffer mode\n");
}

void action_set_token_debug() {
    do_token_debug = true;
    printf("Token debug mode\n");
}

struct option_action_entry option_actions[] = {
    { "nobuffer", action_set_nobuffer },
    { "debug", action_set_token_debug },
};

ssize_t option_entries = sizeof(option_actions) / sizeof(struct option_action_entry);

int main(int argc, char **argv) {

    if (isatty(fileno(stdin))) {
        printf("Nightingale shell\n");
        interactive = true;
    } else {
        interactive = false;
    }

    int pid = getpid();
    setpgid(pid, pid);

    signal(SIGINT, signal_handler);

    for (int i=1; i<argc; i++) {
        for (int opt=0; opt<option_entries; opt++) {
            if (strcmp(argv[i], option_actions[opt].name) == 0) {
                option_actions[opt].action();
                break;
            }
        }
    }

    while (handle_one_line() == 0) {}

    return 0;
}

