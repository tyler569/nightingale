
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

typedef uint16_t vid_char;

static struct cursor cursor = { .x = 0, .y = 0 };
static struct term_color color = { .fg = COLOR_WHITE, .bg = COLOR_DARK_GREY };
static vid_char default_bg_char;

vid_char *video_memory = (void *)0xB8000;

vid_char video_buffer[80 * 25];

static void flush_video_buffer() {
    memmove(video_memory, video_buffer, 80 * 25 * sizeof(vid_char));
}

static vid_char pack_vid_char(char a, struct term_color c) {
    return (c.fg | c.bg << 4) << 8 | a;
}

static void update_hw_cursor(/* struct cursor or global implicit ? */) {
    // TODO
}

static size_t cursor_offset(/* struct cursor or global implicit ? */) {
    return cursor.y * TERMINAL_WIDTH + cursor.x;
}

static void clear_terminal() {
    wmemset(video_buffer, default_bg_char, 80*25);
    flush_video_buffer();
}

static void scroll(size_t n) {
    if (n > 25) {
        clear_terminal();
        return;
    }
    memmove(video_buffer, video_buffer + (80 * n), 80 * (25 - n) * 2);
    wmemset(video_buffer + (80 * (25 - n)), default_bg_char, 80 * n), 
    flush_video_buffer();
}

static void print_char(char c) {
    vid_char vc = pack_vid_char(c, color);
    if (c == '\n') {
        cursor.x = 0;
        cursor.y += 1;
    } else {
        video_buffer[cursor_offset()] = vc;
        video_memory[cursor_offset()] = vc;
        cursor.x += 1;
    }
    if (cursor.x == TERMINAL_WIDTH) {
        cursor.x = 0;
        cursor.y += 1;
    }
    if (cursor.y == TERMINAL_HEIGHT) {
        scroll(1);
        cursor.y -= 1;
    }
}

static int print_count(const char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        print_char(buf[i]);
    }
    return len;
}

void term_init() {
    default_bg_char = pack_vid_char(' ', color);
    clear_terminal();
    term.write = &print_count;
}

