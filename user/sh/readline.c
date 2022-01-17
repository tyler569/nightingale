// vim: ts=4 sw=4 sts=4 :

#include "readline.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct history_item history_base = {0};
struct history_item *history_top = &history_base;

// Line manipulation

struct line_state {
    int length;
    int cursor;
    int size;
};

#define LEFT(i) "\x1B[" i "D"
#define CLEAR_AFTER "\x1B[K"
#define BACKSPACE "\x08 \x08"
#define START_OF_LINE "\x0D"

void print_shell_prompt() {
    printf("$ ");
}

struct line_state clear_line(char *buf, struct line_state state) {
    if (state.length == 0) return state;
    // memset(buf, 0, state.size);
    // \x1B[#D    Move cursor back # characters
    // \x1B[K     Erase rest-of-line
    printf(LEFT("%i") CLEAR_AFTER, state.length);
    state.length = 0;
    state.cursor = 0;
    return state;
}

struct line_state backspace(char *buf, struct line_state state) {
    if (state.length == 0 || state.cursor == 0) return state;
    if (state.cursor == state.length) {
        // backspace off end, easy path
        printf(BACKSPACE);
    } else {
        int after = state.length - state.cursor;
        char *cur = buf + state.cursor;
        memmove(cur - 1, cur, after);
        cur -= 1;
        printf(LEFT("1") "%.*s" CLEAR_AFTER LEFT("%i"), after, cur, after);
    }
    state.cursor -= 1;
    state.length -= 1;

    return state;
}

struct line_state put(char *buf, struct line_state state, char c) {
    if (state.cursor == state.length) {
        // backspace off end, easy path
        buf[state.cursor] = c;
        printf("%c", c);
    } else {
        int after = state.length - state.cursor;
        char *at = buf + state.cursor;
        memmove(at + 1, at, after);
        buf[state.cursor] = c;
        printf(CLEAR_AFTER "%.*s", after + 1, at);
    }
    state.cursor += 1;
    state.length += 1;

    return state;
}

struct line_state load_line(char *buf, char *new, struct line_state state) {
    state = clear_line(buf, state);
    printf("%s", new);
    state.length = state.cursor = strlen(new);
    memcpy(buf, new, state.length);
    return state;
}

void rerender(char *buf, struct line_state state) {
    int left = state.length - state.cursor;
    printf(START_OF_LINE CLEAR_AFTER);
    print_shell_prompt();
    printf("%.*s " LEFT("%i"),
            state.length, buf, left);
}

// History

void store_history_line(char *line_to_store, long len) {
    struct history_item *node = malloc(sizeof(struct history_item));
    node->history_line = strdup(line_to_store);

    node->previous = history_top;
    node->next = NULL;
    history_top->next = node;
    history_top = node;
}

struct line_state load_history_line(char *buf, struct line_state state,
        struct history_item *current) {
    state = clear_line(buf, state);
    if (!current->history_line) return state;
    state = load_line(buf, current->history_line, state);
    return state;
}

// Read line

long read_line_interactive(char *buf, size_t max_len) {
#define CBLEN 128
    struct line_state state = { .size = max_len };
    char cb[CBLEN] = {0};

    struct history_item local = {
        .previous = history_top,
    };
    struct history_item *current = &local;

    while (true) {
        int readlen = 0;
        memset(cb, 0, CBLEN);
        readlen = read(STDIN_FILENO, cb, CBLEN);
        if (readlen == -1) {
            perror("read()");
            return -1;
        }
        if (readlen == 0) return -1;

        if (cb[0] == '\x1b') {
        esc_seq:
            if (strcmp(cb, "\x1b[A") == 0) { // up arrow
                if (current->previous) current = current->previous;
                state = load_history_line(buf, state, current);
                continue;
            } else if (strcmp(cb, "\x1b[B") == 0) { // down arrow
                if (current->next) current = current->next;
                state = load_history_line(buf, state, current);
                continue;
            } else if (strcmp(cb, "\x1b[C") == 0) { // right arrow
                if (state.cursor < state.length) {
                    printf("\x1b[C");
                    state.cursor += 1;
                }
                continue;
            } else if (strcmp(cb, "\x1b[D") == 0) { // left arrow
                if (state.cursor > 0) {
                    printf("\x1b[D");
                    state.cursor -= 1;
                }
                continue;
            } else {
                if (strlen(cb) > 3) {
                    printf("unknown escape-sequence %s\n", &cb[1]);
                    continue;
                }
                int rl = read(STDIN_FILENO, &cb[readlen], 1);
                if (rl > 0) {
                    readlen += rl;
                } else {
                    perror("read()");
                }
                goto esc_seq;
            }
        }

#define CONTROL(n) ((n) - 'a' + 1)

        for (int i = 0; i < readlen; i++) {
            char escape_status = 0;
            char c = cb[i];

            switch (c) {
            case 0x7f: // backspace
                state = backspace(buf, state);
                continue;
            case CONTROL('k'):
                state = clear_line(buf, state);
                continue;
            case CONTROL('n'):
                if (current->previous) current = current->previous;
                state = load_history_line(buf, state, current);
                continue;
            case CONTROL('h'):
                if (current->next) current = current->next;
                state = load_history_line(buf, state, current);
                continue;
            case CONTROL('r'):
                rerender(buf, state);
                continue;
            case '\n':
                goto done;
            }

            if (state.length + 1 == max_len)  goto done;

            if (!isprint(c)) {
                printf("^%c", c + '@');
                continue;
            }

            state = put(buf, state, c);
            cb[i] = 0;
        }
    }

done:
    buf[state.length] = 0;
    if (state.length > 0) store_history_line(buf, state.length);
    putchar('\n');
    return state.length;
#undef CBLEN
}

long read_line_simple(FILE *file, char *buf, size_t limit) {
    if (feof(file)) return -1;

    char *v = fgets(buf, limit, file);
    if (v == NULL) return -1;

    int ix = strlen(buf);

    // EVIL HACK FIXME
    if (buf[ix - 1] == '\n') {
        buf[ix - 1] = '\0';
        ix -= 1;
    }
    return 0;
}
