#pragma once
#ifndef NGSH_READLINE_H
#define NGSH_READLINE_H

// vim: ts=4 sw=4 sts=4 :


struct history_item {
    struct history_item *previous;
    struct history_item *next;
    char *history_line;
};

void clear_line(char *buf, long *ix);

void backspace(char *buf, long *ix);

void load_line(char *buf, long *ix, char *new_line);

void store_history_line(char *line_to_store, long len);

void load_history_line(char *buf, long *ix, struct history_item *);

long read_line_interactive(char *buf, size_t max_len);

long read_line_simple(char *buf, size_t limit);

#endif // NGSH_READLINE_H
