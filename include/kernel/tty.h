
#pragma once
 
#include <stddef.h>

#define TAB_WIDTH 8
 
void terminal_initialize();
void terminal_putchar(char c);
int terminal_write(char* data, size_t size);
void terminal_writestring(char* data);
void terminal_cursor_update(size_t row, size_t column);
void terminal_scroll();
 
