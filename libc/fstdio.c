#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum file_state {
    STATE_READ = 1,
    STATE_WRITE = 2,
    STATE_APPEND = 3,
    STATE_RW_MASK = 3,
    STATE_MALLOC = 4,
};

struct _FILE {
    int fd;
    enum file_state mode;
    int unget_char;
    int eof;
    int error;
    off_t offset;

    size_t buffer_size;
    size_t buffer_length;
    char *buffer_data;
    // size_t buffer_cursor;
    int buffer_mode;
    char internal_data[BUFSIZ];
};

void print_file(FILE *stream) {
    printf(
        "FILE { .fd = %i, .mode = %i, .eof = %i, .error = %i, .buffer_mode = %i, ...}",
        stream->fd,
        stream->mode,
        stream->eof,
        stream->error,
        stream->buffer_mode
    );
}

char *file_buffer(FILE *stream) {
    if (stream->buffer_data) {
        return stream->buffer_data;
    } else {
        return stream->internal_data;
    }
}

void advance_buffer(FILE *stream, size_t n) {
    char *data = file_buffer(stream);

    memmove(data, data + n, stream->buffer_length - n);
    stream->buffer_length -= n;
}

size_t buffer_space(FILE *stream) {
    return stream->buffer_size - stream->buffer_length;
}

bool buffer_is_full(FILE *stream) {
    return buffer_space(stream) == 0;
}

bool buffer_has_newline(FILE *stream) {
    return memchr(file_buffer(stream), '\n', stream->buffer_length) != 0;
}

int add_to_buffer(FILE *stream, const char *buf, size_t len) {
    char *data = file_buffer(stream);
    size_t max_add = min(buffer_space(stream), len);
    memcpy(data + stream->buffer_length, buf, max_add);
    stream->buffer_length += max_add;
    return max_add;
}

int write_to_file(FILE *stream) {
    if (stream->buffer_length == 0)
        return 0;

    char *data = file_buffer(stream);
    int written = write(stream->fd, data, stream->buffer_length);
    if (written < 0) {
        stream->error = written;
        return written;
    }
    advance_buffer(stream, written);
    return written;
}

int write_to_file_if_full(FILE *stream) {
    if (!buffer_is_full(stream))
        return 0;
    return write_to_file(stream);
}

void flush_buffer(FILE *stream) {
    if (stream->mode == STATE_READ) {
        // discard any buffered, unconsumed data.
        stream->buffer_length = 0;
    } else {
        write_to_file(stream);
    }
}

int write_line_to_file(FILE *stream) {
    char *data = file_buffer(stream);
    char *end = memchr(data, '\n', stream->buffer_length);
    if (end) {
        end += 1;
    } else {
        end = data + stream->buffer_length;
    }
    int written = write(stream->fd, data, end - data);
    if (written < 0) {
        stream->error = written;
        return written;
    }
    advance_buffer(stream, written);
    return written;
}

int write_lines_to_file(FILE *stream) {
    int total_written = 0;
    while (
        (buffer_is_full(stream) || buffer_has_newline(stream)) &&
        !stream->error
    ) {
        total_written += write_line_to_file(stream);
    }
    return total_written;
}

int read_line_into_buffer(FILE *stream) {
    char *data = file_buffer(stream);
    int total_read = 0;
    while (!buffer_has_newline(stream) && !stream->error && !stream->eof) {
        size_t max_read = buffer_space(stream);
        int nread = read(
            stream->fd,
            data + stream->buffer_length,
            max_read
        );
        if (nread < 0) {
            stream->error = nread;
            return nread;
        }
        if (nread == 0) {
            stream->eof = 1;
            return total_read;
        }
        total_read += nread;
        stream->buffer_length += nread;
    }
    return total_read;
}

int read_into_buffer(FILE *stream) {
    char *data = file_buffer(stream);
    int total_read = 0;
    while (!buffer_is_full(stream) && !stream->error && !stream->eof) {
        size_t max_read = buffer_space(stream);
        int nread = read(
            stream->fd,
            data + stream->buffer_length,
            max_read
        );
        if (nread < 0) {
            stream->error = nread;
            return nread;
        }
        if (nread == 0) {
            stream->eof = 1;
            return total_read;
        }
        total_read += nread;
        stream->buffer_length += nread;
    }
    return total_read;
}

int copy_line(char *out, FILE *stream, int max) {
    char *data = file_buffer(stream);
    char *end = memchr(data, '\n', stream->buffer_length);
    if (end) {
        end += 1;
    } else if (buffer_is_full(stream)) {
        end = data + stream->buffer_length;
    } else {
        return 0;
    }

    size_t ncopy = min(end - data, max - 1);
    strncpy(out, data, ncopy);
    out[ncopy] = 0;
    advance_buffer(stream, ncopy);
    return ncopy;
}

int copy_buffer(char *out, FILE *stream, int max) {
    char *data = file_buffer(stream);
    char *end = data + stream->buffer_length;

    size_t ncopy = min(end - data, max);
    memcpy(out, data, ncopy);
    advance_buffer(stream, ncopy);
    return ncopy;
}

FILE *stdin = &(FILE){
    .fd = 0,
    .mode = STATE_READ,
    .buffer_mode = _IOLBF,
    .buffer_size = BUFSIZ,
};
FILE *stdout = &(FILE){
    .fd = 1,
    .mode = STATE_WRITE,
    .buffer_mode = _IOLBF,
    .buffer_size = BUFSIZ,
};
FILE *stderr = &(FILE){
    .fd = 2,
    .mode = STATE_WRITE,
    .buffer_mode = _IONBF,
    .buffer_size = BUFSIZ,
};

size_t fread(void *buf, size_t n, size_t cnt, FILE *stream) {
    size_t len = n * cnt;
    size_t n_read = 0;

    if (stream->unget_char) {
        memcpy(buf, &stream->unget_char, 1);
        len -= 1;
        stream->unget_char = 0;
        n_read += 1;
    }

    switch (stream->buffer_mode) {
    case _IONBF:
        n_read += read(stream->fd, buf, len);
        break;
    case _IOLBF:
        read_line_into_buffer(stream);
        n_read += copy_line(buf, stream, len);
        break;
    case _IOFBF:
        read_into_buffer(stream);
        n_read += copy_buffer(buf, stream, len);
        break;
    default:
        stream->error = 100;
        return -1;
    }

    return n_read;
}

char *fgets(char *s, int size, FILE *stream) {
    read_line_into_buffer(stream);
    if (copy_line(s, stream, size)) {
        return s;
    } else {
        return NULL;
    }
}

int fwrite(const void *buf, size_t n, size_t cnt, FILE *stream) {
    size_t len = n * cnt;
    int total_written = 0;
    int written = 0;

    switch (stream->buffer_mode) {
    case _IONBF:
        total_written = write(stream->fd, buf, len);
        break;
    case _IOLBF:
        while (len > 0) {
            written = add_to_buffer(stream, buf, len);
            total_written += written;
            len -= written;
            write_lines_to_file(stream);
        }
        break;
    case _IOFBF:
        while (len > 0) {
            written = add_to_buffer(stream, buf, len);
            total_written += written;
            len -= written;
            write_to_file_if_full(stream);
        }
        break;
    }
    return total_written;
}

static FILE *_fopen_to(const char *filename, const char *mode, FILE *f) {
    bool was_malloced = f->mode & STATE_MALLOC;
    int open_flags = 0;
    enum file_state smode = 0;

    if (mode[0] == 'r') {
        open_flags |= O_RDONLY;
        smode = STATE_READ;
    } else if (mode[0] == 'w') {
        open_flags |= O_WRONLY | O_CREAT | O_TRUNC;
        smode = STATE_WRITE;
    } else if (mode[0] == 'a') {
        open_flags |= O_WRONLY;
        smode = STATE_APPEND;
    }

    int fd = open(filename, open_flags);
    if (fd < 0)
        return NULL;

    f->buffer_size = BUFSIZ;
    f->buffer_mode = _IOFBF;
    f->mode = smode | (was_malloced ? STATE_MALLOC : 0);
    f->fd = fd;
    return f;
}

FILE *fdopen(int fd, const char *mode) {
    FILE *f;
    enum file_state smode = 0;

    if (mode[0] == 'r') {
        smode = STATE_READ;
    } else if (mode[0] == 'w') {
        smode = STATE_WRITE;
    } else if (mode[0] == 'a') {
        smode = STATE_APPEND;
    }

    f = malloc(sizeof(FILE));
    memset(f, 0, sizeof(FILE));
    f->buffer_size = BUFSIZ;
    f->buffer_mode = _IOFBF;
    f->mode = smode | STATE_MALLOC;
    f->fd = fd;

    return f;
}

FILE *fopen(const char *filename, const char *mode) {
    FILE *f = malloc(sizeof(FILE));
    memset(f, 0, sizeof(FILE));
    FILE *ret = _fopen_to(filename, mode, f);
    if (!ret) {
        free(f);
        return NULL;
    }
    ret->mode |= STATE_MALLOC;
    return ret;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
    flush_buffer(stream);
    close(stream->fd);
    if (!filename) {
        // In this case, freopen is to change the mode associated with the
        // stream without changing the file. It is implementation defined
        // which (if any) mode changes are allowed. I currently define
        // the set to be empty.
        return NULL;
    }
    stream = _fopen_to(filename, mode, stream);
    return stream;
}

int fflush(FILE *f) {
    flush_buffer(f);
    return 0;
}

int fclose(FILE *f) {
    flush_buffer(f);
    int close_result = close(f->fd);
    if (f->mode & STATE_MALLOC)
        free(f);
    return close_result;
}

void clearerr(FILE *stream) {
    stream->error = 0;
}

int feof(FILE *stream) {
    return stream->eof && stream->buffer_length == 0;
}

int ferror(FILE *stream) {
    return stream->error != 0;
}

int fileno(FILE *stream) {
    return stream->fd;
}

int fseek(FILE *stream, long offset, int whence) {
    stream->eof = 0;
    stream->buffer_length = 0;
    stream->offset = 0;
    stream->unget_char = 0;
    int status = seek(stream->fd, offset, whence);
    if (status < 0) {
        return -1;
    }

    stream->offset = status;
    stream->eof = false;

    /*
       if (whence == SEEK_END && offset = 0)
            stream->eof = true;
     */
    return 0;
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
    if (feof(f))
        return EOF;
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

void setbuf(FILE *stream, char *buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}

void setbuffer(FILE *stream, char *buf, size_t size) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, size);
}

void setlinebuf(FILE *stream) {
    setvbuf(stream, NULL, _IOLBF, BUFSIZ);
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
    stream->buffer_data = buf;
    stream->buffer_size = size != 0 ? size : BUFSIZ;
    stream->buffer_mode = mode;
    return 0;
}
