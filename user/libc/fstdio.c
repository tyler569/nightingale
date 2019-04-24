
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

FILE *fopen(const char *filename, const char *mode) { return NULL; }

int fputs(const char *str, FILE *stream) {
        return write(stream->fd, str, strlen(str));
}

int vfprintf(FILE *stream, const char *format, va_list args) {
        return vdprintf(stream->fd, format, args);
}

int fprintf(FILE *stream, const char *format, ...) {
        va_list args;
        va_start(args, format);

        return vdprintf(stream->fd, format, args);
}

int fread(char *buf, size_t n, size_t cnt, FILE *stream) {
        return read(stream->fd, buf, n * cnt);
}

int fwrite(const char *buf, size_t n, size_t cnt, FILE *stream) {
        return write(stream->fd, buf, n * cnt);
}
