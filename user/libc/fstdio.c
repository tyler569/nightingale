
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct _FILE {
        int fd;

        int buf_len;
        char buffer[BUFSIZ];
        int eof;
        off_t offset;
};

void print_file(FILE *f) {
        printf("FILE %i {\n", f->fd);
        printf("    buffer: \"...\", len: %i\n", f->buf_len);
        printf("    at eof: %s\n", f->eof ? "true" : "false");
        printf("    offset: %lli\n", f->offset);
        printf("}\n");
}
void print_buffer(FILE *f) {
        for (int i=0; i<24; i++) {
                printf("%02hhx ", f->buffer[i]);
        }
        printf("\n");
}

FILE *stdin = &(FILE){.fd = 0};
FILE *stdout = &(FILE){.fd = 1};
FILE *stderr = &(FILE){.fd = 2};

FILE *fopen(const char *filename, const char *mode) {
        int fd = open(filename, 0); if (fd < 0) {
                return NULL;
        }

        FILE *f = malloc(sizeof(FILE));
        memset(f, 0, sizeof(FILE));

        f->fd = fd;
        return f;
}

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

static void read_into_buf(FILE *f) {
        int siz = read(f->fd, f->buffer + f->buf_len,
                       BUFSIZ - f->buf_len);
        if (siz == 0) {
                f->eof = true;
        }
        f->buf_len += siz;
}

static void read_count(FILE *f, size_t len) {
        while (len > 0 && !f->eof && BUFSIZ != f->buf_len) {
                size_t can_read = min(len, BUFSIZ - f->buf_len);

                int siz = read(f->fd, f->buffer + f->buf_len, can_read);
                if (siz == 0) {
                        f->eof = true;
                }

                f->buf_len += siz;
                len -= siz;
        }
}

static void read_until(FILE *f, char c) {
        // read until a character is found in the buffer
        while (!strchr(f->buffer, c) && !f->eof) {
                read_into_buf(f);
        }
}

static size_t consume_buffer(FILE *f, char *output, ssize_t len) {
        len = min(len, f->buf_len);
        memcpy(output, f->buffer, len);
        memmove(f->buffer, f->buffer + len, len);
        f->buf_len -= len;
        memset(f->buffer + f->buf_len, 0, BUFSIZ - f->buf_len);
        return len;
}

size_t fread(char *buf, size_t n, size_t cnt, FILE *stream) {
        read_count(stream, n * cnt);
        size_t used = consume_buffer(stream, buf, n * cnt);
        buf[used] = '\0';
        return used;
}

char *fgets(char *s, int size, FILE *stream) {
        char *after;
        read_until(stream, '\n');
        after = strchr(stream->buffer, '\n');
        size_t will_read = min(size, (size_t)(after - stream->buffer + 1));

        size_t used = consume_buffer(stream, s, will_read);

        s[used] = '\0';
        return &s[used];
}


int fwrite(const char *buf, size_t n, size_t cnt, FILE *stream) {
        return write(stream->fd, buf, n * cnt);
}

int fflush(FILE *f) {
        return 0;
}

int fclose(FILE *f) {
        return 0;
}

void clearerr(FILE *stream) {
        return;
}

int feof(FILE *stream) {
        return stream->eof && stream->buf_len == 0;
}

int ferror(FILE *stream) {
        return 0;
}

int fileno(FILE *stream) {
        return stream->fd;
}

int fseek(FILE *stream, long offset, int whence) {
        stream->eof = false;
        stream->offset = 0; // something;
        return seek(stream->fd, offset, whence);
}

int ftell(FILE *stream) {
        return stream->offset;
}
