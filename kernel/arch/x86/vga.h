
#pragma once
#ifndef NIGHTINGALE_VGA_H
#define NIGHTINGALE_VGA_H

#include <basic.h>
#include "uart.h"

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

/*
typedef struct Abstract_Terminal {
    usize (*write)(const char *buf, usize len);
    void (*clear)();
    void (*color)(Color fg, Color bg);
    char (*readc)();
} Abstract_Terminal;
*/

usize vga_write(const char *buf, usize len);
void vga_clear();
void vga_flush();
void vga_set_color(Color fg, Color bg);

#endif
