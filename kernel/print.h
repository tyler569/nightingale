
#pragma once
#ifndef NIGHTINGALE_PRINT_H
#define NIGHTINGALE_PRINT_H

#include <basic.h>

void debug_print_mem(int, void *);
void debug_dump(void *);

usize printf(const char *format, ...);

/* TODO:
i32 kprint(const char *string);
i32 kprintln(const char *string);

i32 kformat(char *buf, const char *format, ...);

i32 kerror(const char *format, ...);
*/

#endif
