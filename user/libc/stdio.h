
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <basic.h>
#include <stddef.h>

#define stdin 0
#define stdout 1
#define stderr 2

ssize_t puts(const char *str);
ssize_t printf(const char *format, ...);

char getchar(void);

#endif
