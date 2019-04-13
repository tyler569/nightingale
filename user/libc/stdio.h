
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <basic.h>
#include <stdarg.h>
#include <stddef.h>

#define stdin 0
#define stdout 1
#define stderr 2

int puts(const char *str);

int vsprintf(char* buf, const char* format, va_list args);
int vprintf(const char* format, va_list args);
int sprintf(char* buf, const char* format, ...);
int printf(const char* format, ...);

char getchar(void);

#endif
