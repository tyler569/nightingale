
#pragma once
#ifndef NIGHTINGALE_TERMINAL_H
#define NIGHTINGALE_TERMINAL_H

#include <basic.h>
#include <cpu/uart.h>

struct abstract_terminal {
    i32 (*write)(const char *buf, usize len);
    // void (*color)() // TODO: how?
    // char (*readc)() // TODO: how?
};

void term_vga_init();
void term_serial_init();

struct abstract_terminal term_vga;
struct abstract_terminal term_serial;

#endif
