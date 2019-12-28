
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
#include "token.h"

struct sh_command {
        struct sh_command *next;

        char **args;
        char *arg_buf;
        int output;
        int input;
};

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


void clear_line(char *buf, long *ix) {
        while (*ix > 0) {
                *ix -= 1;
                buf[*ix] = '\0';
                printf("\x08 \x08");
        }
}

void backspace(char *buf, long *ix) {
        if (*ix == 0)
                return;
        *ix -= 1;
        buf[*ix] = '\0';
        printf("\x08 \x08");
}

void load_line(char *buf, long *ix, char *new_line) {
        clear_line(buf, ix);
        while (*new_line) {
                buf[*ix] = *new_line;
                printf("%c", *new_line);
                new_line += 1;
                *ix += 1;
        }
}

typedef struct hist hist;
struct hist {
        hist *previous;
        hist *next;
        char *history_line;
};

hist hist_base = {0};
hist *hist_top = 0;

void store_history_line(char *line_to_store, long len, hist *node) {
        char *line = malloc(len + 5);
        strcpy(line, line_to_store);
        node->history_line = line;

        hist_top = node;
}

void load_history_line(char *buf, long *ix, hist *current) {
        clear_line(buf, ix);
        if (!current->history_line)
                return;
        load_line(buf, ix, current->history_line);
}

long read_line(char *buf, size_t max_len) {
        long ix = 0;
        int readlen = 0;
        char cb[256] = {0};

        if (!hist_top)
                hist_top = &hist_base;

        hist *this_node = zmalloc(sizeof(hist));
        hist *current = this_node;
        hist_top->next = current;
        current->previous = hist_top;
        current->history_line = "";

        while (true) {
                memset(cb, 0, 256);
                readlen = read(STDIN_FILENO, cb, 256);
                if (readlen == -1) {
                        perror("read()");
                        return -1;
                }
                if (readlen == 0) {
                        return -1;
                }

                if (cb[0] == '\x1b') {
esc_seq:
                        if (strcmp(cb, "\x1b[A") == 0) { // up arrow
                                if (current->previous)
                                        current = current->previous;
                                load_history_line(buf, &ix, current);
                                continue;
                        } else if (strcmp(cb, "\x1b[B") == 0) { // down arrow
                                if (current->next)
                                        current = current->next;
                                load_history_line(buf, &ix, current);
                                continue;
                        } else {
                                if (strlen(cb) > 3) {
                                        printf("unknown escape-sequence %s\n",
                                                        &cb[1]);
                                        continue;
                                }
                                int rl = read(STDIN_FILENO, &cb[readlen], 1);
                                if (rl > 0)  readlen += rl;
                                else perror("read()");
                                goto esc_seq;
                        }
                }

                for (int i = 0; i < readlen; i++) {
                        char escape_status = 0;
                        char c = cb[i];

                        switch (c) {
                                case 0x7f: // backspace
                                        backspace(buf, &ix);
                                        continue;
                                case 0x0b: // ^K
                                        clear_line(buf, &ix);
                                        continue;
                                case 0x0c: // ^L
                                        load_line(buf, &ix, "heapdbg both");
                                        continue;
                                case 0x0e: // ^N
                                        if (current->previous)
                                                current = current->previous;
                                        load_history_line(buf, &ix, current);
                                        continue;
                                case 0x08: // ^H
                                        if (current->next)
                                                current = current->next;
                                        load_history_line(buf, &ix, current);
                                        continue;
                                case '\n':
                                        goto done;
                        }

                        if (ix + 1 == max_len)
                                goto done; // continue;

                        if (!isprint(c)) {
                                printf("(%hhx)", c);
                                continue;
                        }

                        buf[ix++] = c;
                        buf[ix] = '\0';
                        if (interactive)
                                putchar(c);
                        cb[i] = 0;
                }
        }

done:
        if (ix > 0) {
                // printf("storing history\n");
                store_history_line(buf, ix, this_node);
        } else {
                free(this_node);
        }
        if (interactive)
                putchar('\n');
        return ix;
}

long read_line_simple(char *buf, size_t limit) {
        // if (!hist_top) {
        //         hist_top = &hist_base;
        // }

        // hist *this_node = malloc(sizeof(hist));
        // hist *current = this_node;
        // hist_top->next = current;
        // current->previous = hist_top;
        // current->history_line = "";

        int ix = read(STDIN_FILENO, buf, limit);
        if (ix <= 0) {
                return -1;
        }

        // EVIL HACK FIXME
        if (buf[ix-1] == '\n') {
                buf[ix-1] = '\0';
                ix -= 1;
        }

        // if (ix > 0) {
        //         store_history_line(buf, ix, this_node);
        // } else {
        //         free(this_node);
        // }
        return ix;
}

struct sh_command *parse_line(struct vector *tokens, ssize_t index, int next_input) {
        // point of this is to track if a | is valid or not.
        // | is not valid at the start of a string or following another |.
        enum parse_state {
                CONTINUE,
                START,
        };
        enum parse_state state = START;

        struct sh_command *ret = malloc(sizeof(struct sh_command));
        ret->next = NULL;
        ssize_t arg_ix = 0;
        ssize_t arg_num = 0;
        ret->args = calloc(32, sizeof(char*));
        ret->arg_buf = malloc(4096);

        ret->input = 0;
        ret->output = 0;

        if (next_input)
                ret->input = next_input;

        ssize_t i = index;
        for (; i<tokens->len; i++) {
                Token *tok = vec_get(tokens, i);
                switch(tok->type) {
                        case TOKEN_HASH:
                                return ret;
                        case token_string: // FALLTHROUGH
                        case token_ident:
                                state = CONTINUE;
                                ret->args[arg_num++] = ret->arg_buf + arg_ix;
                                strcpy(ret->arg_buf + arg_ix, tok->string);
                                arg_ix += strlen(tok->string) + 1;
                                break;
                        case TOKEN_INPUT: {
                                Token *input_file = vec_get(tokens, i + 1);
                                i++;
                                if (!input_file) {
                                        printf("unexpected EOF, (needed file <)");
                                        return NULL; // LEAKS
                                }
                                char *name = input_file->string;
                                int fd = open(name, O_RDONLY);
                                if (fd < 0) {
                                        printf("%s does not exist or is not writeable\n", name);
                                        return NULL; // LEAKS
                                }
                                ret->input = fd;
                                break;
                        }
                        case TOKEN_OUTPUT: {
                                Token *output_file = vec_get(tokens, i + 1);
                                i++;
                                if (!output_file) {
                                        printf("unexpected EOF, (needed file >)");
                                        return NULL; // LEAKS
                                }
                                char *name = output_file->string;
                                int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                if (fd < 0) {
                                        printf("%s does not exist or is not writeable\n", name);
                                        return NULL; // LEAKS
                                }
                                ret->output = fd;
                                break;
                        }
                        case TOKEN_PIPE: {
                                if (state == START) {
                                        printf("unexpected '|', (needed command)\n");
                                        return NULL; // LEAKS
                                }
                                int pipe_fds[2];
                                int err = pipe(pipe_fds);
                                if (err < 0) {
                                        perror("pipe()");
                                        exit(1);
                                }
                                ret->output = pipe_fds[1];
                                ret->next = parse_line(tokens, i + 1, pipe_fds[0]);
                                return ret;
                        }
                        default:
                                printf("error: unhandled token ");
                                debug_print_token(tok);
                                printf("\n");
                }
        }

        return ret;
}

void recursive_free_sh_command(struct sh_command *cmd) {
        if (cmd->next)
                recursive_free_sh_command(cmd->next);
        free(cmd->args);
        free(cmd->arg_buf);
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
                if (read_line(cmdline, 256) == -1) {
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

        if (strncmp("history", arg_0, 7) == 0) {
                hist *hl = hist_top;
                for (; hl->history_line; hl = hl->previous) {
                        printf("%s\n", hl->history_line);
                }
                return 0;
        }

        if (strncmp("exit", arg_0, 4) == 0) {
                return 1;
        }

        int ret_val = run(instruction);

        recursive_free_sh_command(instruction);

        if (ret_val != 0)
                printf("-> %i\n", ret_val);

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

