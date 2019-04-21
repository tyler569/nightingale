
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <ng/basic.h>
#include <stdarg.h>
#include <stddef.h>

#define stdin_fd 0
#define stdout_fd 1
#define stderr_fd 2

#define BUFSIZ 0x1000 /* shrug */
#define _IONBF 1
#define _IOLBF 2
#define _IOFBF 3
#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

enum {
    EOF = -1,
};

typedef struct _FILE {
    int fd;
} FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int puts(const char *str);

int vsprintf(char* buf, const char* format, va_list args);
int vprintf(const char* format, va_list args);
int sprintf(char* buf, const char* format, ...);
int printf(const char* format, ...);

char getchar(void);

inline int fflush() { return 0; }

FILE* fopen(const char* name, const char* mode);
int fprintf(FILE* file, const char* format, ...);

#endif
