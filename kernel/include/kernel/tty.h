
#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H
 
#include <stddef.h>

#define TAB_WIDTH 8
 
void terminal_initialize();
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_cursor_update(size_t row, size_t column);
void terminal_scroll();
 
#endif // _KERNEL_TTY_HS

