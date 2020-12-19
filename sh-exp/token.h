#ifndef SH_TOKEN_H
#define SH_TOKEN_H

#include "list.h" // <list.h>
#include <stdbool.h>

enum token_type {
    TOKEN_INPUT,  // '<'
    TOKEN_OUTPUT, // '>'
    TOKEN_PIPE,   // '|'
    TOKEN_AMPERSAND,
    TOKEN_STRING,
    TOKEN_VAR,
};

struct token {
    enum token_type type;
    const char *string;
    off_t begin, end;
    list_node node;
};

void token_print(struct token *t);
void token_fprint(FILE *, struct token *t);
bool tokenize(const char *string, list_head *out);
char *token_strdup(struct token *t);
char *token_strcpy(char *dest, struct token *t);

#endif
