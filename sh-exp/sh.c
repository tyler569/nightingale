#include <assert.h>
#include <ctype.h>
#include "list.h" // <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "parse.h"

bool token_debug = true;
bool ast_debug = true;

int eval(struct node *);

int main() {
    char buffer[1024];
    list tokens;
    fprintf(stderr, "$ ");
    while (fgets(buffer, 1024, stdin)) {
        list_init(&tokens);
        if (tokenize(buffer, &tokens)) {
            if (token_debug) {
                list_for_each(struct token, t, &tokens, node) {
                    token_print(t);
                    printf("\n");
                }
            }
            struct node *node = parse(&tokens);
            if (node) {
                if (ast_debug) {
                    node_print(node);
                }
                eval(node);
            }
        }
        fprintf(stderr, "$ ");
    }
}
