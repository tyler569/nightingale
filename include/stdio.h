#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <basic.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#define BUFSIZ 0x1000 /* shrug */
#define _IONBF 1
#define _IOLBF 2
#define _IOFBF 3

#define L_tmpnam 50 /* uh */

enum file_type {
    FT_CHARDEV,
    FT_TTY,
    FT_BUFFER,
    FT_SOCKET,
    FT_DIRECTORY,
    FT_PIPE,
    FT_PROC,
    FT_PROC_THREAD,
};

enum file_mode {
    ALL_READ = 0004,
    ALL_WRITE = 0002,
    ALL_EXEC = 0001,
    GRP_READ = 0040,
    GRP_WRITE = 0020,
    GRP_EXEC = 0010,
    USR_READ = 0400,
    USR_WRITE = 0200,
    USR_EXEC = 0100,

    SUID = 01000,
    SGID = 02000,
};

enum seek_modes {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

#define MAX_FILENAME 64

#ifndef __kernel__

#define EOF (-1)

struct _FILE;
typedef struct _FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#endif // ifndef __kernel__

int puts(const char *str);

int vsnprintf(char *buf, size_t len, const char *format, va_list args);
int vsprintf(char *buf, const char *format, va_list args);
int vprintf(const char *format, va_list args);
int sprintf(char *buf, const char *format, ...);
int snprintf(char *buf, size_t len, const char *format, ...);
int printf(const char *format, ...);

#ifndef __kernel__

int vdprintf(int fd, const char *buf, va_list args);
int dprintf(int fd, const char *format, ...);
int close(int fd);
int getchar(void);
int putchar(int c);
int getc(FILE *f);
int fgetc(FILE *f);
int ungetc(int c, FILE *f);
int putc(int c, FILE *f);
int fputc(int c, FILE *f);
int fputs(const char *c, FILE *f);
int fflush(FILE *f);
FILE *fopen(const char *name, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *name, const char *mode, FILE *stream);
int vfprintf(FILE *file, const char *format, va_list args);
int fprintf(FILE *file, const char *format, ...);
int fputs(const char *str, FILE *stream);
size_t fwrite(const void *s, size_t size, size_t len, FILE *file);
size_t fread(void *s, size_t size, size_t len, FILE *file);
char *fgets(char *str, int num, FILE *stream);
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fileno(FILE *stream);
int fclose(FILE *stream);
int getc_unlocked(FILE *stream);
int getchar_unlocked(void);
int putc_unlocked(int c, FILE *stream);
int putchar_unlocked(int c);
void clearerr_unlocked(FILE *stream);
int feof_unlocked(FILE *stream);
int ferror_unlocked(FILE *stream);
int fileno_unlocked(FILE *stream);
int fflush_unlocked(FILE *stream);
int fgetc_unlocked(FILE *stream);
int fputc_unlocked(int c, FILE *stream);
size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream);
char *fgets_unlocked(char *s, int n, FILE *stream);
int fputs_unlocked(const char *s, FILE *stream);

// TODO squad
FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);
FILE *tmpfile(void);
char *tmpnam(char *s);
void flockfile(FILE *filehandle);
int ftrylockfile(FILE *filehandle);
void funlockfile(FILE *filehandle);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);
void setbuf(FILE *stream, char *buf);
void setbuffer(FILE *stream, char *buf, size_t size);
void setlinebuf(FILE *stream);
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
int remove(const char *pathname);
int rename(const char *oldpath, const char *newpath);
int sscanf(const char *s, const char *format, ...);

#endif // ifndef __kernel__

#endif // _STDIO_H_
