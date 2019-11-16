
#pragma once
#ifndef NGSH_TOKEN_H
#define NGSH_TOKEN_H

#include <stdbool.h>
#include <stddef.h>

typedef enum TokenType {
    token_invalid = 0,

    TOKEN_OUTPUT,
    TOKEN_INPUT,
    TOKEN_PIPE,
    TOKEN_VAR,

    token_string,
    token_ident,
} TokenType;

typedef struct Location {
    size_t index;
} Location;

typedef struct Token {
    TokenType type;
    char* string;

    Location loc;
} Token;

/*
typedef struct TokenList {
    Token* v;
    struct TokenList* next;
} TokenList;
*/

/* Functions put the new Token at the provided pointer, which is assumed
 * to be preallocated to sizeof(Token)
 *
 * The number of characters consummed is returned. */

struct vector *tokenize_string(char* program);

size_t make_integer_token(Token* t, char* st, Location loc);
size_t make_string_token(Token* t, char* st, Location loc);
size_t make_ident_token(Token* t, char* st, Location loc);

void debug_print_token(Token* t);
void print_token_vector(struct vector *);

void print_tokens(char *);

void free_token(Token *t);
void free_token_vector(struct vector *tokens);

#endif // NGSH_TOKEN_H

