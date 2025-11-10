#include "token.h"
#include <assert.h>
#include <ctype.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token_info {
	enum token_type type;
	bool is_simple;
	const char *name;
	const char *value;
} token_info[] = {
	[TOKEN_APPEND] = { TOKEN_APPEND, true, "append", ">>" },
	[TOKEN_ERRAPPEND] = { TOKEN_ERRAPPEND, true, "errappend", "2>>" },
	[TOKEN_ERROUTPUT] = { TOKEN_ERROUTPUT, true, "erroutput", "2>" },
	[TOKEN_INPUT] = { TOKEN_INPUT, true, "input", "<" },
	[TOKEN_OUTPUT] = { TOKEN_OUTPUT, true, "output", ">" },
	[TOKEN_OR] = { TOKEN_OR, true, "or", "||" },
	[TOKEN_AND] = { TOKEN_AND, true, "and", "&&" },
	[TOKEN_PIPE] = { TOKEN_PIPE, true, "pipe", "|" },
	[TOKEN_AMPERSAND] = { TOKEN_AMPERSAND, true, "ampersand", "&" },
	[TOKEN_OPAREN] = { TOKEN_OPAREN, true, "oparen", "(" },
	[TOKEN_CPAREN] = { TOKEN_CPAREN, true, "cparen", ")" },
	[TOKEN_SEMICOLON] = { TOKEN_SEMICOLON, true, "semicolon", ";" },
	[TOKEN_STRING] = { TOKEN_STRING, false, "string", "" },
	[TOKEN_VAR] = { TOKEN_VAR, false, "var", "" },
};

void token_fprint(FILE *f, struct token *t) {
	fprintf(f, "token(%s, \"%.*s\")", token_info[t->type].name,
		(int)(t->end - t->begin), t->string + t->begin);
}

void token_print(struct token *t) {
	token_fprint(stdout, t);
}

char *token_strdup(struct token *t) {
	return strndup(t->string + t->begin, t->end - t->begin);
}

char *token_strcpy(char *dest, struct token *t) {
	strncpy(dest, t->string + t->begin, t->end - t->begin);
	return dest + t->end - t->begin;
}

bool isident(char c) {
	return isalnum(c) || c == '_' || c == '-' || c == '/' || c == '.'
		|| c == '?';
}

static struct token *make_token(const char *string, const char *begin,
	const char *end, enum token_type type) {
	struct token *t = malloc(sizeof(struct token));
	t->type = type;
	t->string = string;
	t->begin = begin - string;
	t->end = end - string;
	t->node = (list) { 0 };
	return t;
}

static void skip_whitespace(const char **cursor) {
	while (isspace(**cursor))
		(*cursor)++;
}

static void ident_end(const char **cursor) {
	while (isident(**cursor))
		(*cursor)++;
}

static void line_end(const char **cursor) {
	while (**cursor && **cursor != '\n')
		(*cursor)++;
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
		if (**cursor == '\\')
			(*cursor)++;
		(*cursor)++;
	}
	if (**cursor)
		(*cursor)++;
}

bool tokenize(const char *string, list *out) {
	const char *cursor = string;
	const char *begin;
	struct token *t;

	while (*cursor) {
		t = nullptr;
		if (isspace(*cursor))
			skip_whitespace(&cursor);

		for (size_t i = 0; i < ARRAY_LEN(token_info); i++) {
			if (!token_info[i].is_simple)
				continue;
			const char *tv = token_info[i].value;
			size_t tv_len = strlen(tv);
			if (strncmp(cursor, tv, tv_len) == 0) {
				t = make_token(
					string, cursor, cursor + tv_len, token_info[i].type);
				cursor += tv_len;
				goto next;
			}
		}

		switch (*cursor) {
		case '"': // FALLTHROUGH
		case '\'':
			begin = cursor;
			string_end(&cursor);
			t = make_token(string, begin + 1, cursor - 1, TOKEN_STRING);
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
			if (isident(*cursor)) {
				begin = cursor;
				ident_end(&cursor);
				t = make_token(string, begin, cursor, TOKEN_STRING);
			} else {
				fprintf(stderr, "Unexpected '%c' at position %zi\n", *cursor,
					cursor - string);
				fprintf(stderr, " > %s\n", string);
				fprintf(stderr, "   ");
				for (int i = 0; i < cursor - string; i++) {
					fprintf(stderr, " ");
				}
				fprintf(stderr, "^\n");
				return false;
			}
		}
	next:
		if (t)
			list_append(out, &t->node);
	}

	return true;
}
