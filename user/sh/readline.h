#pragma once

#include <stdio.h>

struct history_item {
	struct history_item *previous;
	struct history_item *next;
	char *history_line;
};

struct line_state;

struct line_state clear_line(char *buf, struct line_state state);
struct line_state backspace(char *buf, struct line_state state);
struct line_state load_line(char *buf, char *new_line, struct line_state state);
void store_history_line(char *line_to_store, long len);
struct line_state load_history_line(
	char *buf, struct line_state state, struct history_item *current);
long read_line_interactive(char *buf, size_t max_len);
long read_line_simple(FILE *file, char *buf, size_t limit);

