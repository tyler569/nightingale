
#pragma once
#ifndef NIGHTINGALE_TERMINAL_H
#define NIGHTINGALE_TERMINAL_H

#include <basic.h>
#include <cpu/uart.h>

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

void term_vga_init();
void term_serial_init();

usize vga_write(const char *buf, usize len);
void vga_clear();
void vga_set_color(Color fg, Color bg);

// extern Abstract_Terminal term_vga;

/* TODO move to term_serial
Abstract_Terminal term_serial = {
    .write = NULL,
    .color = NULL,
    .clear = NULL,
    .readc = NULL, // Read from serial interrupt ring buffer
};
*/

#endif
