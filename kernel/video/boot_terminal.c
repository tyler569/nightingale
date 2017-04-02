
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

static struct cursor cursor = { .x = 0, .y = 0 };
static uint16_t default_bg_char = ' ' | 0x07;
static uint16_t *video_memory = (void *)0xB8000;
static uint16_t video_buffer[80 * 25];

static void flush_video_buffer() {
    memmove(video_memory, video_buffer, 80 * 25 * sizeof(uint16_t));
}

uint16_t pack_vga_character(char a, enum color fg, enum color bg) {
    return (fg | bg << 4) << 8 | a;
}

/*
static void update_hw_cursor( struct cursor or global implicit ? ) {
    // TODO ?
}
*/

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

int terminal_write(const char *buf, size_t len) {
    for (size_t i=0; i<len; i++) {
        uint16_t vc = pack_vga_character(buf[i], COLOR_LIGHT_GREY, COLOR_BLACK);
        if (buf[i] == '\n') {
            cursor.x = 0;
            cursor.y += 1;
        } // there are other cases to handle here: \t, \0, others
        else {
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
    return len;
}

void term_init() {
    clear_terminal();
    term.write = &terminal_write;
}

