
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <ng/basic.h>
#include <sys/types.h>
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

struct _FILE;
typedef struct _FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int puts(const char *str);

int vsprintf(char *buf, const char *format, va_list args);
int vdprintf(int fd, const char *buf, va_list args);
int vprintf(const char *format, va_list args);
int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t len, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int printf(const char *format, ...);

int seek(int fd, off_t offset, int whence);

char getchar(void);
char getc(FILE *f);

int fflush(FILE *f);

FILE *fopen(const char *name, const char *mode);
int vfprintf(FILE *file, const char *format, va_list args);
int fprintf(FILE *file, const char *format, ...);
int fputs(const char *str, FILE *stream);
int fwrite(const char *s, size_t size, size_t len, FILE *file);
size_t fread(char *s, size_t size, size_t len, FILE *file);

char *fgets(char *str, int num, FILE *stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fileno(FILE *stream);

#endif
