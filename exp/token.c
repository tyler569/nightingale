#include <assert.h>
#include <ctype.h>
#include "list.h" //<list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum token_type {
    TOKEN_INPUT,  // '<'
    TOKEN_OUTPUT, // '>'
    TOKEN_PIPE,   // '|'
    TOKEN_STRING,
    TOKEN_VAR,
};

struct token {
    enum token_type type;
    const char *string;
    off_t begin, end;
    list_node node;
};

void token_print(struct token *t) {
    printf("token(");
    switch (t->type) {
    case TOKEN_INPUT: printf("input"); break;
    case TOKEN_OUTPUT: printf("output"); break;
    case TOKEN_PIPE: printf("pipe"); break;
    case TOKEN_STRING: printf("string"); break;
    case TOKEN_VAR: printf("var"); break;
    default: printf("invalid");
    }
    printf(", \"%.*s\")\n", (int)(t->end-t->begin), t->string+t->begin);
}

bool isident(char c) {
    return isalnum(c) || c == '_' || c == '-';
}

struct token *make_token(const char *string, const char *begin, const char *end,
                         enum token_type type) {
    struct token *t = malloc(sizeof(struct token));
    t->type = type;
    t->string = string;
    t->begin = begin - string;
    t->end = end - string;
    t->node = (list_head){0};
    return t;
}

static void skip_whitespace(const char **cursor) {
    while (isspace(**cursor)) (*cursor)++;
}

static void ident_end(const char **cursor) {
    while (isident(**cursor)) (*cursor)++;
}

static void string_end(const char **cursor) {
    char delim = **cursor;
    assert(delim == '"' || delim == '\'');
    (*cursor)++;
    while (**cursor && **cursor != delim) {
        // It doesn't matter what comes after (*the first*) \, it can never
        // end the string. This advances two places, so if it's 
        //     "\\"
        // we'll end up on the closing delimeter.
        if (**cursor == '\\') (*cursor)++;
        (*cursor)++;
    }
    if (**cursor) (*cursor)++;
}

bool tokenize(const char *string, list_head *out) {
    const char *cursor = string;
    const char *begin;
    struct token *t;

    while (*cursor) {
        t = NULL;
        switch (*cursor) {
        case '|':
            t = make_token(string, cursor, cursor+1, TOKEN_PIPE);
            cursor++;
            break;
        case '>':
            t = make_token(string, cursor, cursor+1, TOKEN_OUTPUT);
            cursor++;
            break;
        case '<':
            t = make_token(string, cursor, cursor+1, TOKEN_INPUT);
            cursor++;
            break;
        case '"': // FALLTHROUGH
        case '\'':
            begin = cursor;
            string_end(&cursor);
            t = make_token(string, begin+1, cursor-1, TOKEN_STRING);
            break;
        case '$':
            cursor++; // '$'
            begin = cursor;
            ident_end(&cursor);
            t = make_token(string, begin, cursor, TOKEN_VAR);
            break;
        default:
            if (isspace(*cursor)) {
                skip_whitespace(&cursor);
            } else if (isident(*cursor)) {
                begin = cursor;
                ident_end(&cursor);
                t = make_token(string, begin, cursor, TOKEN_STRING);
            } else {
                fprintf(stderr, "Unexpected '%c' at position %zi\n", *cursor, cursor - string);
                fprintf(stderr, "\n%s", string);
                for (int i=0; i<cursor - string; i++) {
                    fprintf(stderr, " ");
                }
                fprintf(stderr, "^\n");
                return false;
            }
        }
        
        if (t) {
            list_append(out, &t->node);
        }
    }

    return true;
}

int main() {
    char buffer[1024];
    list_head tokens;
    while (fgets(buffer, 1024, stdin)) {
        list_init(&tokens);
        if (tokenize(buffer, &tokens)) {
            list_for_each(struct token, t, &tokens, node) {
                token_print(t);
            }
        }
    }
}
