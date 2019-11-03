
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

struct _FILE {
        int fd;

        int unget_char;

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
        int open_flags = 0;
        if (strchr(mode, 'r'))  open_flags |= O_RDONLY;
        if (strchr(mode, 'w'))  open_flags |= O_WRONLY;

        int fd = open(filename, open_flags); if (fd < 0) {
                return NULL;
        }

        FILE *f = malloc(sizeof(FILE));
        memset(f, 0, sizeof(FILE));

        f->fd = fd;
        return f;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
        fclose(stream);
        
        int open_flags = 0;
        if (strchr(mode, 'r'))  open_flags |= O_RDONLY;
        if (strchr(mode, 'w'))  open_flags |= O_WRONLY;

        int fd = open(filename, open_flags); if (fd < 0) {
                return NULL;
        }

        stream->fd = fd;
        stream->eof = 0;
        stream->buffer[0] = '\0';
        stream->buf_len = 0;
        stream->offset = 0;
        stream->unget_char = 0;

        return stream;
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
        if (siz < 0) {
                perror("read()");
        }
        if (siz == 0) {
                f->eof = true;
        }
        f->buf_len += siz;
}

static void read_count(FILE *f, size_t len) {
        while (len > 0 && !f->eof && BUFSIZ != f->buf_len) {
                size_t can_read = min(len, BUFSIZ - f->buf_len);

                int siz = read(f->fd, f->buffer + f->buf_len, can_read);
                if (siz < 0) {
                        perror("read()");
                }
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
        int did_unget = 0;
        if (f->unget_char) {
                output[0] = f->unget_char;
                len -= 1;
                did_unget = 1;
                f->unget_char = 0;
        }
        len = min(len, f->buf_len);
        memcpy(output, f->buffer, len);
        memmove(f->buffer, f->buffer + len, len);
        f->buf_len -= len;
        memset(f->buffer + f->buf_len, 0, BUFSIZ - f->buf_len);
        // consume_buffer has to return the actual amount of bytes
        // placed in *output, other things rely on that. That said,
        // it is very convenienet to dec len if we unget a char,
        // as that indicated the smaller buffer available. Here,
        // we put that dec back and indicate the extra character
        // added to the buffer in the event of an unget.
        return len + (did_unget) ? 2 : 0;
}

size_t fread(void *buf_, size_t n, size_t cnt, FILE *stream) {
        char *buf = buf_;
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


int fwrite(const void *buf, size_t n, size_t cnt, FILE *stream) {
        return write(stream->fd, buf, n * cnt);
}

int fflush(FILE *f) {
        return 0;
}

int fclose(FILE *f) {
        return close(f->fd);
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

long ftell(FILE *stream) {
        return stream->offset;
}

int fseeko(FILE *stream, off_t offset, int whence) {
        return fseek(stream, offset, whence);
}

off_t ftello(FILE *stream) {
        return ftell(stream);
}

int getc(FILE *f) {
        char c;
        fread(&c, 1, 1, f);
        return c;
}

int fgetc(FILE *f) {
        return getc(f);
}

int getchar(void) {
        return getc(stdin);
}

int ungetc(int c, FILE *f) {
        if (f->unget_char) {
                // Pushed-back characters will be returned in reverse order;
                // only one pushback is guaranteed. 
                // ungetc() returns c on success, or EOF on error. 
                return EOF;
        }
        f->unget_char = c;
        return c;
}

int putc(int c, FILE *f) {
        char buf = c;
        return fwrite(&buf, 1, 1, f);
}

int fputc(int c, FILE *f) {
        return putc(c, f);
}

int putchar(int c) {
        return putc(c, stdout);
}

int fputs(const char *str, FILE *stream) {
        size_t len = strlen(str);
        return fwrite(str, 1, len, stream);
}

