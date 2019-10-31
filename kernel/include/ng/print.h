
#pragma once
#ifndef NIGHTINGALE_PRINT_H
#define NIGHTINGALE_PRINT_H

#include <ng/basic.h>
#include <stdarg.h>

// prints to both vga and serial
void raw_print(const char *buf, size_t len);

void debug_print_mem(size_t, void *);
void debug_dump(void *);
void debug_dump_after(void *);

size_t print_ptr(void *);

size_t vsprintf(char *buf, const char *format, va_list args);
size_t sprintf(char *buf, const char *format, ...);
size_t vprintf(const char *format, va_list args);
size_t printf(const char *format, ...);

#endif
