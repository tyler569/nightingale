
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

#define stdin 0
#define stdout 1
#define stderr 1

size_t puts(const char *str);
size_t printf(const char *format, ...);
size_t fprintf(int fd, const char* format, ...);

#endif
