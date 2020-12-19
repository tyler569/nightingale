#include <assert.h>
#include "list.h" // <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"

void fprint_command(FILE *f, struct command *command) {
}

void fprint_pipeline(FILE *f, struct pipeline *pipeline) {
}

void fprint_node(FILE *f, struct node *node) {
}

static struct command *parse_command(list *tokens) {
    struct command *command = calloc(1, sizeof(struct command));
    char *arg_space = malloc(2048);
    char **argv_space = malloc(64 * sizeof(char *));
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
            argv_cursor++;
            break;
        case TOKEN_INPUT:
            __list_pop_front(tokens);
            t = list_pop_front(struct token, node, tokens);
            command->stdin_file = token_strdup(t);
            break;
        case TOKEN_OUTPUT:
            __list_pop_front(tokens);
            t = list_pop_front(struct token, node, tokens);
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
    struct node *n;
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
        default:
            fprintf(stderr, "Unexpected token ");
            token_fprint(stderr, t);
            fprintf(stderr, " at position %zi\n\n", t->end - t->begin);
            fprintf(stderr, "%s", t->string);
            for (int i=0; i<t->end - t->begin; i++) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "^\n");
        }
    }
}

