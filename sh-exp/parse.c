#include <assert.h>
#include "list.h" // <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"

void fprint_ws(FILE *f, int c) {
    fprintf(f, "%.*s", c, "                                                  ");
}

static void fprint_command(FILE *f, struct command *command, int depth) {
    fprintf(f, "command { ");
    for (char **arg = command->argv; *arg; arg++) {
        fprintf(f, "[%s] ", *arg);
    }
    if (command->stdin_file) {
        fprintf(f, "(<%s) ", command->stdin_file);
    }
    if (command->stdout_file) {
        fprintf(f, "(>%s) ", command->stdout_file);
    }
    if (command->stderr_file) {
        fprintf(f, "(2>%s) ", command->stderr_file);
    }
    fprintf(f, "}\n");
}

static void fprint_pipeline(FILE *f, struct pipeline *pipeline, int depth) {
    fprintf(f, "pipeline {\n");
    list_for_each(struct command, c, &pipeline->commands, node) {
        fprint_ws(f, (depth+1)*4);
        fprint_command(f, c, depth+1);
    }
    fprint_ws(f, depth*4);
    fprintf(f, "}\n");
}

void fprint_node(FILE *f, struct node *node, int depth) {
    switch (node->type) {
    case NODE_PIPELINE:
        fprintf(f, "node pipeline {\n");
        fprint_ws(f, (depth+1)*4);
        fprint_pipeline(f, node->pipeline, depth+1);
        fprint_ws(f, depth*4);
        fprintf(f, "}\n");
        break;
    case NODE_BINOP:
        fprintf(f, "node binop {\n");
        fprint_ws(f, (depth+1)*4);
        fprintf(f, "op: ");
        switch (node->op) {
        case NODE_THEN: fprintf(f, "THEN\n"); break;
        default: fprintf(f, "UNKNOWN (%i)\n", node->op); break;
        }
        fprint_ws(f, (depth+1)*4);
        fprintf(f, "left: ");
        fprint_node(f, node->left, depth+1);
        fprint_ws(f, (depth+1)*4);
        fprintf(f, "right: ");
        fprint_node(f, node->right, depth+1);
        fprint_ws(f, depth*4);
        fprintf(f, "}\n");
        break;
    default:
        fprintf(f, "node unknown (%i) {???}\n", node->type);
    }
}

void node_print(struct node *node) {
    fprint_node(stdout, node, 0);
}

static void unexpected_token(struct token *t) {
    fprintf(stderr, "Unexpected token ");
    token_fprint(stderr, t);
    fprintf(stderr, " at position %zi\n\n", t->begin);
    fprintf(stderr, "%s", t->string);
    fprint_ws(stderr, t->begin);
    fprintf(stderr, "^\n");
}

static struct command *parse_command(list *tokens) {
    struct command *command = calloc(1, sizeof(struct command));
    char *arg_space = calloc(1, 2048);
    char **argv_space = calloc(64, sizeof(char *));
    char *arg_cursor = arg_space;
    char **argv_cursor = argv_space;

    command->argv = argv_space;
    command->args = arg_space;

    struct token *t;
    while (!list_empty(tokens)) {
        t = list_head(struct token, node, tokens);
        switch (t->type) {
        case TOKEN_STRING:
            __list_pop_front(tokens);
            *argv_cursor = arg_cursor;
            arg_cursor = token_strcpy(arg_cursor, t);
            arg_cursor++; // leave a '\0'
            argv_cursor++;
            break;
        case TOKEN_INPUT:
            __list_pop_front(tokens);
            t = list_pop_front(struct token, node, tokens);
            if (command->stdin_file) free(command->stdin_file);
            command->stdin_file = token_strdup(t);
            break;
        case TOKEN_OUTPUT:
            __list_pop_front(tokens);
            t = list_pop_front(struct token, node, tokens);
            if (command->stdout_file) free(command->stdout_file);
            command->stdout_file = token_strdup(t);
            break;
        default:
            goto out;
        }
    }
    out:
    return command;
}

static struct node *parse_pipeline(list *tokens) {
    struct pipeline *pipeline = calloc(1, sizeof(struct pipeline));
    struct node *node = calloc(1, sizeof(struct node));
    node->pipeline = pipeline;
    node->type = NODE_PIPELINE;

    list_init(&pipeline->commands);

    struct command *command = parse_command(tokens);
    list_append(&pipeline->commands, &command->node);

    struct token *t;
    while (!list_empty(tokens)) {
        t = list_head(struct token, node, tokens);
        switch (t->type) {
        case TOKEN_PIPE:
            // eat, parse another command onto the pipeline
            __list_pop_front(tokens);
            struct command *command = parse_command(tokens);
            list_append(&pipeline->commands, &command->node);
            break;
        case TOKEN_AMPERSAND:
            __list_pop_front(tokens);
            fprintf(stderr, "Background command ignored, & is TODO\n");
            goto out;
        default:
            // break switch and while
            goto out;
        }
    }
out:
    return node;
}

struct node *parse(list *tokens) {
    struct node *n = NULL;
    struct token *t;
    while (!list_empty(tokens)) {
        t = list_head(struct token, node, tokens);
        switch (t->type) {
        case TOKEN_STRING:
            if (!n) {
                n = parse_pipeline(tokens);
            } else {
                struct node *new_root = calloc(1, sizeof(struct node));
                new_root->type = NODE_BINOP;
                new_root->op = NODE_THEN;
                new_root->left = n;
                new_root->right = parse_pipeline(tokens);
                n = new_root;
            }
            break;
        case TOKEN_SEMICOLON:
            __list_pop_front(tokens);
            // Do nothing -- pipeline [ ] pipeline is interpreted as ';',
            // and this way a ';' at the end of a line does not intruduce
            // an extra node for no reason. Something ended the pipeline,
            // and that's all that matters. This is just the same as something
            // like an '&'
            break;
        default:
            unexpected_token(t);
            // TODO either handle error or do cleanup
            return NULL;
        }
    }
    return n;
}
