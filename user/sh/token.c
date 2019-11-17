
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include "token.h"

typedef struct TokenStr {
    TokenType t;
    char* name;
} TokenStr;

void debug_print_token(Token*);

#define lcc_error(l, ...) printf(__VA_ARGS__); exit(1);

/* @Note:
 * These are iterated through in order, and therefore
 * any shorter matches must be after longer ones to
 * prevent ambiguities.
 * i.e.:
 * += matching token_plus, token_equal, instead
 * of token_plus_equal.
 */
TokenStr literal_token_names[] = {
    { TOKEN_OUTPUT, ">" },
    { TOKEN_INPUT, "<" },
    { TOKEN_PIPE, "|" },
    { TOKEN_VAR, "$" },
};

const int n_special_tokens = sizeof(literal_token_names) / sizeof(*literal_token_names);

bool is_not_special(char c) {
    for (int i=0; i<n_special_tokens; i++) {
        if (c == literal_token_names[i].name[0])
            return false;
        if (isspace(c))
            return false;
        if (!isprint(c))
            return false;
    }
    return true;
}

size_t make_string_token(Token* t, char* st, Location loc)
{
    char* data;
    char* test;
    size_t length = 0;

    /* For now, when creating a string token I require the whole thing (or more)
     * to be passed in -- including quotation marks.
     *
     * This may not be a good idea, but ultimately depends on how I end up
     * handling the actual file reading.
     *
     * TODO I am leaving off escape handling for now. */

    //if (st[0] != '"') {
    //    lcc_compiler_error("make_string_token called without being string");
    //}

    char delim = *st;
    test = st + 1;
    while (*test++ != delim) 
        length += 1;
    /* TODO: factor out escape code handling.
     *
     * while (*test != '"') {
        if (*test == '\\') {
            test++;
            continue;
        }
        length += 1;
    } */

    data = malloc(length + 1);
    strncpy(data, st + 1, length);
    data[length] = '\0';

    t->type = token_string;
    t->string = data;
    t->loc = loc;

    return length + 2;
}

size_t make_ident_token(Token* t, char* st, Location loc)
{
    char* data;
    char* test;
    size_t length = 0;

    test = st;
    while (*test && is_not_special(*test)) {
        test++;
        length += 1;
    }

    data = malloc(length + 1);
    strncpy(data, st, length);
    data[length] = '\0';

    t->type = token_ident;
    t->string = data;
    t->loc = loc;

    return length;
}

size_t make_other_token(Token* t, char* st, Location loc)
{
    int i;
    size_t max = strlen(st);
    size_t cur_len;
    t->type = token_invalid;
    t->string = NULL;

    for (i = 0; i < n_special_tokens; i++) {
        cur_len = strlen(literal_token_names[i].name);

        if (cur_len > max) {
            continue;
        }
        if (strncmp(literal_token_names[i].name, st, cur_len) == 0) {
            t->type = literal_token_names[i].t;
            /* Without this break, it will keep searching everything
             * and ultimately match the *last* match, even if it
             * is shorter
             */
            break;
        }
    }

    if (t->type == token_invalid) {
        return 0;
    }

    t->loc = loc;

    return cur_len;
}

void debug_print_token(Token* t)
{
    switch (t->type) {
    case token_invalid:
        printf("[Null token - probably uninitialized]");
        break;
    case token_string:
        printf("[String(%s) @ %lu]", t->string, t->loc.index);
        break;
    case token_ident:
        printf("[Ident(%s) @ %lu]", t->string, t->loc.index);
        break;
    default:
        for (int i = 0; i < n_special_tokens; i++) {
            if (t->type == literal_token_names[i].t) {
                printf("[ op: %s ]", literal_token_names[i].name);
                return;
            }
        }
        printf("[Unknown - this is a bug.]");
        /*
         * lcc_compiler_error("Unknown token type");
         */
    }
}

struct vector *tokenize_string(char* program)
{
    Location loc = { 1 };
    size_t index;
    size_t program_length = strlen(program);
    size_t tmp_len;

    struct vector *tokens = malloc(sizeof(struct vector));
    vec_init(tokens, Token);

    Token tmp;

    for (index = 0; index < program_length;) {
        if (program[index] == ' ') {
            loc.index += 1;
            index += 1;
            /* Continue so we don't make an empty token */
            continue;
        } else if (program[index] == '\t') {
            loc.index += 4;
            index += 1;
            /* Continue so we don't make an empty token */
            continue;
        } else if (program[index] == '"') {
            tmp_len = make_string_token(&tmp, program + index, loc);

            index += tmp_len;
            loc.index += tmp_len;
        } else if (program[index] == '\'') {
            tmp_len = make_string_token(&tmp, program + index, loc); 

            index += tmp_len;
            loc.index += tmp_len;
        } else if (is_not_special(program[index])) {
            // check if it's |>< first
            tmp_len = make_other_token(&tmp, program + index, loc);

            if (tmp_len == 0) {
                tmp_len = make_ident_token(&tmp, program + index, loc);
            }

            index += tmp_len;
            loc.index += tmp_len;
        } else {
            tmp_len = make_other_token(&tmp, program + index, loc);
            /* @Debug token length
            debug_print_token(&tmp);
            printf("\ntmp_len: %zu\n\n", tmp_len);
            */

            if (tmp_len == 0) {
                lcc_error(loc, "Unrecognized token");
            }

            index += tmp_len;
            loc.index += tmp_len;
        }
    
        vec_push(tokens, &tmp);
    }

    return tokens;
}

void print_token_vector(struct vector *tokens) {
    for (int i=0; i<tokens->len; i++) {
        debug_print_token(vec_get(tokens, i));
        printf("\n");
    }
}

void print_tokens(char *string) {
    struct vector *tokens = tokenize_string(string);
    print_token_vector(tokens);
    free_token_vector(tokens);
}

void free_token(Token *t) {
    if (t->string)  free(t->string);
}

void free_token_vector(struct vector *tokens) {
    for (int i=0; i<tokens->len; i++) {
        Token *t = vec_get(tokens, i);
        free_token(t);
    }
    vec_free(tokens);
}

