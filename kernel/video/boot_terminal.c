
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "terminal.h"

#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25

struct cursor {
    size_t x;
    size_t y;
};

enum color {
    COLOR_BLACK             = 0,
    COLOR_BLUE              = 1,
    COLOR_GREEN             = 2,
    COLOR_CYAN              = 3,
    COLOR_RED               = 4,
    COLOR_MAGENTA           = 5,
    COLOR_BROWN             = 6,
    COLOR_LIGHT_GREY        = 7,
    COLOR_DARK_GREY         = 8,
    COLOR_LIGHT_BLUE        = 9,
    COLOR_LIGHT_GREEN       = 10,
    COLOR_LIGHT_CYAN        = 11,
    COLOR_LIGHT_RED         = 12,
    COLOR_LIGHT_MAGENTA     = 13,
    COLOR_LIGHT_BROWN       = 14,
    COLOR_WHITE             = 15,
};

struct term_color {
    enum color fg;
    enum color bg;
};

struct character {
    char ch;
    uint8_t color;
};


static struct cursor cursor = { .x = 0, .y = 0 };
static struct term_color color = { .fg = COLOR_WHITE, .bg = COLOR_GREEN };

struct character *video_memory = (void *)0xB8000;

static uint8_t pack_color(struct term_color c) {
    return c.fg | c.bg << 4;
}

static void update_hw_cursor(/* struct cursor or global implicit ? */) {
    // TODO
}

static size_t cursor_offset(/* struct cursor or global implicit ? */) {
    return cursor.y * TERMINAL_WIDTH + cursor.x;
}

static void clear_terminal() {
    struct character c = { .ch = ' ', .color = pack_color(color) };
    for (size_t i=0; i<TERMINAL_HEIGHT * TERMINAL_WIDTH; i++) {
        video_memory[i] = c;
    }
}

static void scroll(size_t n) {
    if (n > 25) {
        clear_terminal();
        return;
    }
}

static void print_char(struct character c) {
    if (c.ch == '\n') {
        cursor.x = 0;
        cursor.y += 1;
    } else {
        video_memory[cursor_offset()] = c;
        cursor.x += 1;
    }
    if (cursor.x == TERMINAL_WIDTH) {
        cursor.x = 0;
        cursor.y += 1;
        if (cursor.y == TERMINAL_HEIGHT) {
            scroll(1);
        }
    }
}

static int print_count(char *buf, size_t len) {
    struct character c = { .ch = '\0', .color = pack_color(color) };
    for (size_t i=0; i<len; i++) {
        c.ch = buf[i];
        print_char(c);
    }
    return len;
}

struct abstract_terminal boot_terminal_init() {
    clear_terminal();
    struct abstract_terminal s = { .write = &print_count };
    return s;
}

