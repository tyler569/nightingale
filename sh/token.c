#include <assert.h>
#include <ctype.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

void token_fprint(FILE *f, struct token *t) {
    fprintf(f, "token(");
    switch (t->type) {
    case TOKEN_INPUT: fprintf(f, "input"); break;
    case TOKEN_OUTPUT: fprintf(f, "output"); break;
    case TOKEN_PIPE: fprintf(f, "pipe"); break;
    case TOKEN_AND: fprintf(f, "and"); break;
    case TOKEN_OR: fprintf(f, "or"); break;
    case TOKEN_OPAREN: fprintf(f, "oparen"); break;
    case TOKEN_CPAREN: fprintf(f, "cparen"); break;
    case TOKEN_AMPERSAND: fprintf(f, "ampersand"); break;
    case TOKEN_SEMICOLON: fprintf(f, "semicolon"); break;
    case TOKEN_STRING: fprintf(f, "string"); break;
    case TOKEN_VAR: fprintf(f, "var"); break;
    default: fprintf(f, "invalid");
    }
    fprintf(f, ", \"%.*s\")", (int)(t->end-t->begin), t->string+t->begin);
}

void token_print(struct token *t) {
    token_fprint(stdout, t);
}

char *token_strdup(struct token *t) {
    return strndup(t->string+t->begin, t->end-t->begin);
}

char *token_strcpy(char *dest, struct token *t) {
    strncpy(dest, t->string+t->begin, t->end-t->begin);
    return dest + t->end - t->begin;
}

bool isident(char c) {
    return isalnum(c) || c == '_' || c == '-' || c == '/' || c == '.' ||
        c == '?';
}

static struct token *make_token(const char *string, const char *begin,
                                const char *end, enum token_type type) {
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

static void line_end(const char **cursor) {
    while (**cursor && **cursor != '\n') (*cursor)++;
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
            if (cursor[1] == '|') {
                t = make_token(string, cursor, cursor+2, TOKEN_OR);
                cursor += 2;
            } else {
                t = make_token(string, cursor, cursor+1, TOKEN_PIPE);
                cursor++;
            }
            break;
        case '>':
            t = make_token(string, cursor, cursor+1, TOKEN_OUTPUT);
            cursor++;
            break;
        case '<':
            t = make_token(string, cursor, cursor+1, TOKEN_INPUT);
            cursor++;
            break;
        case '&':
            if (cursor[1] == '&') {
                t = make_token(string, cursor, cursor+2, TOKEN_AND);
                cursor += 2;
            } else {
                t = make_token(string, cursor, cursor+1, TOKEN_AMPERSAND);
                cursor++;
            }
            break;
        case ';':
            t = make_token(string, cursor, cursor+1, TOKEN_SEMICOLON);
            cursor++;
            break;
        case '(':
            t = make_token(string, cursor, cursor+1, TOKEN_OPAREN);
            cursor++;
            break;
        case ')':
            t = make_token(string, cursor, cursor+1, TOKEN_CPAREN);
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
        case '#':
            // discard comment
            line_end(&cursor);
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
                fprintf(stderr, " > %s\n", string);
                fprintf(stderr, "   ");
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
