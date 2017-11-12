
#pragma once

#include <cpu/uart.h>

struct abstract_terminal {
    int (*write)(const char *buf, size_t len);
    // void (*color)() // TODO: how?
    // char (*readc)() // TODO: how?
};

void term_vga_init();
void term_serial_init();

struct abstract_terminal term_vga;
struct abstract_terminal term_serial;

