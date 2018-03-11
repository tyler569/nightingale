
#include <basic.h>
#include <string.h>
#include "vga.h"

#define VGA_XMAX 80
#define VGA_YMAX 25

typedef struct Cursor {
    size_t x;
    size_t y;
} Cursor;

/*
Abstract_Terminal term_vga = {
    .write = vga_write,
    .color = vga_set_color,
    .clear = vga_clear,
    .readc = NULL, // Read from keyboard interrupt ring buffer
};
*/

/* moved to vga.h
typedef enum Color {
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
} Color;
*/

Cursor vga_cursor = { .x = 0, .y = 0 };
Color vga_cur_fg = COLOR_LIGHT_GREY;
Color vga_cur_bg = COLOR_BLACK;

u16 *vga_memory = (void *)0xB8000;
u16 vga_buffer[VGA_XMAX * VGA_YMAX];

// #define __IMMIDIATE_VGA
bool vga_buffer_dirty = false;

void vga_flush() {
    if (vga_buffer_dirty) {
        memmove(vga_memory, vga_buffer, VGA_XMAX * VGA_YMAX * sizeof(u16));
        vga_buffer_dirty = false;
    }
}

static inline u16 vga_pack_char(char a, Color fg, Color bg) {
    return (fg | bg << 4) << 8 | a;
}

/*
void update_hw_cursor(vga_cursor or global implicit ? ) {
    // TODO ?
}
*/

size_t vga_cursor_offset() {
    return vga_cursor.y * VGA_XMAX + vga_cursor.x;
}

int vga_clear() { // rename: clear_vga
    u16 bg_char = vga_pack_char(' ', vga_cur_fg, vga_cur_bg);

    wmemset(vga_buffer, bg_char, VGA_XMAX*VGA_YMAX);
    vga_buffer_dirty = true;
#ifdef __IMMIDIATE_VGA
    vga_flush();
#endif
}

void vga_set_color(Color fg, Color bg) { // rename: set_vga_color
    vga_cur_fg = fg;
    vga_cur_bg = bg;
}

int vga_scroll(int lines) { // rename: scroll_vga
    if (lines > VGA_YMAX) {
        vga_clear();
        return;
    }
    u16 bg_char = vga_pack_char(' ', vga_cur_fg, vga_cur_bg);

    memmove(vga_buffer, vga_buffer + (VGA_XMAX * lines), VGA_XMAX * (VGA_YMAX - lines) * 2);
    wmemset(vga_buffer + (VGA_XMAX * (VGA_YMAX - lines)), bg_char, VGA_XMAX * lines);
    vga_buffer_dirty = true;

#ifdef __IMMIDIATE_VGA
    vga_flush();
#endif

    return lines;
}

size_t vga_write(const char *buf, size_t len) { // rename: write_to_vga
    for (size_t i=0; i<len; i++) {
        u16 vc = vga_pack_char(buf[i], vga_cur_fg, vga_cur_bg);
        if (buf[i] == '\n') {
            vga_cursor.x = 0;
            vga_cursor.y += 1;
        } // there are other cases to handle here: \t, \0, others
        else {
            vga_buffer[vga_cursor_offset()] = vc;
            // vga_memory[vga_cursor_offset()] = vc;
            vga_cursor.x += 1;
        }
        if (vga_cursor.x == VGA_XMAX) {
            vga_cursor.x = 0;
            vga_cursor.y += 1;
        }
        if (vga_cursor.y == VGA_YMAX) {
            vga_scroll(1);
            vga_cursor.y -= 1;
        }
    }
    vga_buffer_dirty = true;

#ifdef __IMMIDIATE_VGA
    vga_flush();
#endif

    return len;
}

