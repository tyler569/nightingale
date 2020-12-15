// vim: ts=4 sw=4 sts=4 :

#include "sh.h"
#include "token.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector.h>

struct sh_command *parse_line(struct vector *tokens, ssize_t index,
                              int next_input) {
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
    ret->args = calloc(32, sizeof(char *));
    ret->arg_buf = malloc(4096);
    ret->arg_buf[0] = 0;

    ret->input = 0;
    ret->output = 0;

    if (next_input) ret->input = next_input;

    ssize_t i = index;
    for (; i < tokens->len; i++) {
        Token *tok = vec_get(tokens, i);
        switch (tok->type) {
        case TOKEN_HASH: return ret;
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
    if (cmd->next) recursive_free_sh_command(cmd->next);
    free(cmd->args);
    free(cmd->arg_buf);
}
