
#pragma once
#ifndef NIGHTINGALE_PRINT_H
#define NIGHTINGALE_PRINT_H

#include <basic.h>

// prints to both vga and serial
void raw_print(const char *buf, usize len);

void debug_print_mem(usize, void *);
void debug_dump(void *);


usize printf(const char *format, ...);

#endif
