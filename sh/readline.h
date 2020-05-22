// vim: ts=4 sw=4 sts=4 :

#ifndef SH_READLINE_H
#define SH_READLINE_H

void clear_line(char *buf, long *ix);
void backspace(char *buf, long *ix);
void load_line(char *buf, long *ix, char *new_line);

void store_history_line(char *line_to_store, long len, hist *node);
void load_history_line(char *buf, long *ix, hist *current);

long read_line_interactive(char *buf, size_t max_len);
long read_line_simple(char *buf, size_t limit);

#endif
