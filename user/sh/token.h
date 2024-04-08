#pragma once
#ifndef NG_SH_TOKEN_H
#define NG_SH_TOKEN_H

#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

enum token_type {
	// These must be sorted so prefixes appear later.
	// '>>' will match '>' twice, so '>>' must be first.
	// This affects the ordering of the token_info struct
	// in token.c, which in turn is the order these are
	// tested against the input string.
	TOKEN_OR, // '||'
	TOKEN_AND, // '&&'
	TOKEN_PIPE, // '|'
	TOKEN_OPAREN, // '('
	TOKEN_CPAREN, // ')'
	TOKEN_APPEND, // '>>'
	TOKEN_ERRAPPEND, // '2>>'
	TOKEN_ERROUTPUT, // '2>'
	TOKEN_INPUT, // '<'
	TOKEN_OUTPUT, // '>'
	TOKEN_AMPERSAND, // '&'
	TOKEN_SEMICOLON, // ';'
	TOKEN_STRING, // "''" | '""'
	TOKEN_VAR, // '$""'
};

struct token {
	enum token_type type;
	const char *string;
	off_t begin, end;
	list_node node;
};

void token_print(struct token *t);
void token_fprint(FILE *, struct token *t);
bool tokenize(const char *string, list *out);
char *token_strdup(struct token *t);
char *token_strcpy(char *dest, struct token *t);

#endif // NG_SH_TOKEN_H
