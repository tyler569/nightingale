// vim: ts=4 sw=4 sts=4 :

#include <stdio.h>
#include <stdlib.h>

struct history_item {
    struct history_item *previous;
    struct history_item *next;
    char *history_line;
};

struct history_item history_base = {0};
struct history_item *history_top = &history_base;

// Line manipulation

void clear_line(char *buf, long *ix) {
    while (*ix > 0) {
        *ix -= 1;
        buf[*ix] = '\0';
        printf("\x08 \x08");
    }
}

void backspace(char *buf, long *ix) {
    if (*ix == 0)
        return;
    *ix -= 1;
    buf[*ix] = '\0';
    printf("\x08 \x08");
}

void load_line(char *buf, long *ix, char *new_line) {
    clear_line(buf, ix);
    while (*new_line) {
        buf[*ix] = *new_line;
        printf("%c", *new_line);
        new_line += 1;
        *ix += 1;
    }
}

// History

void store_history_line(char *line_to_store, long len, hist *node) {
    char *line = malloc(len + 1);
    strcpy(line, line_to_store);
    node->history_line = line;

    list_prepend
}

void load_history_line(char *buf, long *ix, hist *current) {
    clear_line(buf, ix);
    if (!current->history_line)
        return;
    load_line(buf, ix, current->history_line);
}

// Read line

long read_line_interactive(char *buf, size_t max_len) {
    long ix = 0;
    int readlen = 0;
    char cb[256] = {0};

    struct history_item *current = history_top;

    while (true) {
        memset(cb, 0, 256);
        readlen = read(STDIN_FILENO, cb, 256);
        if (readlen == -1) {
            perror("read()");
            return -1;
        }
        if (readlen == 0) {
            return -1;
        }

        if (cb[0] == '\x1b') {
esc_seq:
            if (strcmp(cb, "\x1b[A") == 0) { // up arrow
                if (current->previous)
                    current = current->previous;
                load_history_line(buf, &ix, current);
                continue;
            } else if (strcmp(cb, "\x1b[B") == 0) { // down arrow
                if (current->next)
                    current = current->next;
                load_history_line(buf, &ix, current);
                continue;
            } else {
                if (strlen(cb) > 3) {
                    printf("unknown escape-sequence %s\n", &cb[1]);
                    continue;
                }
                int rl = read(STDIN_FILENO, &cb[readlen], 1);
                if (rl > 0)  readlen += rl;
                else perror("read()");
                goto esc_seq;
            }
        }

        for (int i = 0; i < readlen; i++) {
            char escape_status = 0;
            char c = cb[i];

            switch (c) {
                case 0x7f: // backspace
                    backspace(buf, &ix);
                    continue;
                case 0x0b: // ^K
                    clear_line(buf, &ix);
                    continue;
                case 0x0e: // ^N
                    if (current->previous)
                        current = current->previous;
                    load_history_line(buf, &ix, current);
                    continue;
                case 0x08: // ^H
                    if (current->next)
                        current = current->next;
                    load_history_line(buf, &ix, current);
                    continue;
                case '\n':
                    goto done;
            }

            if (ix + 1 == max_len)
                goto done; // continue;

            if (!isprint(c)) {
                printf("(%hhx)", c);
                continue;
            }

            buf[ix++] = c;
            buf[ix] = '\0';
            putchar(c);
            cb[i] = 0;
        }
    }

done:
    if (ix > 0) {
        struct history_item *node = malloc(sizeof(struct history_node));
        node->previous = history_top;
        node->next = NULL;
        node->history_line = malloc(ix + 1);
        strcpy(node->history_node, buf, ix);
        store_history_line(buf, ix, node);
    }
    putchar('\n');
    return ix;
}

long read_line_simple(char *buf, size_t limit) {
    if (feof(stdin))  return -1;

    char *v = fgets(buf, limit, stdin);
    if (v == NULL) {
        return -1;
    }

    int ix = strlen(buf);

    // EVIL HACK FIXME
    if (buf[ix-1] == '\n') {
        buf[ix-1] = '\0';
        ix -= 1;
    }
    return 0;
}

