#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <ng/common.h>
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
	list_node files_node;
	char internal_data[BUFSIZ];
};

static list __all_files;
FILE *stdin;
FILE *stdout;
FILE *stderr;

void print_file(FILE *stream) {
	printf("FILE { .fd = %i, .mode = %i, .eof = %i, .error = %i, .buffer_mode "
		   "= %i, ...}",
		stream->fd, stream->mode, stream->eof, stream->error,
		stream->buffer_mode);
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

bool buffer_is_full(FILE *stream) { return buffer_space(stream) == 0; }

bool buffer_has_newline(FILE *stream) {
	return memchr(file_buffer(stream), '\n', stream->buffer_length) != 0;
}

int add_to_buffer(FILE *stream, const char *buf, size_t len) {
	char *data = file_buffer(stream);
	size_t max_add = MIN(buffer_space(stream), len);
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

void clear_buffer(FILE *stream) { stream->buffer_length = 0; }

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
	while ((buffer_is_full(stream) || buffer_has_newline(stream))
		&& !stream->error) {
		total_written += write_line_to_file(stream);
	}
	return total_written;
}

int read_line_into_buffer(FILE *stream) {
	char *data = file_buffer(stream);
	int total_read = 0;
	while (!buffer_has_newline(stream) && !stream->error && !stream->eof) {
		size_t max_read = buffer_space(stream);
		int nread = read(stream->fd, data + stream->buffer_length, max_read);
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
		int nread = read(stream->fd, data + stream->buffer_length, max_read);
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

	size_t ncopy = MIN(end - data, max);
	memcpy(out, data, ncopy);
	if (ncopy < max)
		out[ncopy] = 0;
	advance_buffer(stream, ncopy);
	return ncopy;
}

int copy_buffer(char *out, FILE *stream, int max) {
	char *data = file_buffer(stream);
	char *end = data + stream->buffer_length;

	size_t ncopy = MIN(end - data, max);
	memcpy(out, data, ncopy);
	advance_buffer(stream, ncopy);
	return ncopy;
}

size_t fread(void *buf, size_t n, size_t cnt, FILE *stream) {
	size_t len = n * cnt;
	size_t n_read = 0;

	if (stream->unget_char) {
		memcpy(buf, &stream->unget_char, 1);
		len -= 1;
		stream->unget_char = 0;
		n_read += 1;
	}

	if (len == 0) {
		return n_read;
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
		stream->error = EINVAL;
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

size_t fwrite(const void *buf, size_t n, size_t cnt, FILE *stream) {
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
	default:
		stream->error = EINVAL;
		return -1;
	}
	return total_written;
}

static FILE *_new_file(void) {
	struct _FILE *file = malloc(sizeof(struct _FILE));
	memset(file, 0, sizeof(struct _FILE));
	list_init(&file->files_node);
	list_append(&__all_files, &file->files_node);
	return file;
}

static void _delete_file(FILE *stream) {
	list_remove(&stream->files_node);
	free(stream);
}

static int _smode(const char *mode) {
	int smode = 0;

	if (mode[0] == 'r') {
		smode = STATE_READ;
	} else if (mode[0] == 'w') {
		smode = STATE_WRITE;
	} else if (mode[0] == 'a') {
		smode = STATE_APPEND;
	}

	return smode;
}

static int _oflags(const char *mode) {
	int oflags = 0;

	if (mode[0] == 'r') {
		oflags = O_RDONLY;
	} else if (mode[0] == 'w') {
		oflags = O_WRONLY | O_CREAT | O_TRUNC;
	} else if (mode[0] == 'a') {
		oflags = O_WRONLY | O_APPEND;
	}

	return oflags;
}

static FILE *_fopen_to(const char *filename, const char *mode, FILE *f) {
	int open_flags = 0;
	enum file_state smode = 0;

	int fd = open(filename, _oflags(mode));
	if (fd < 0)
		return NULL;

	f->mode = _smode(mode);
	f->buffer_size = BUFSIZ;
	f->buffer_mode = _IOFBF;
	f->fd = fd;
	return f;
}

FILE *fdopen(int fd, const char *mode) {
	FILE *f;

	f = _new_file();
	f->buffer_size = BUFSIZ;
	f->buffer_mode = _IOFBF;
	f->mode = _smode(mode);
	f->fd = fd;

	return f;
}

FILE *fopen(const char *filename, const char *mode) {
	FILE *f = _new_file();
	FILE *ret = _fopen_to(filename, mode, f);
	if (!ret) {
		_delete_file(f);
		return NULL;
	}
	return ret;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
	fflush(stream);
	if (!filename) {
		// In this case, freopen is to change the mode associated with the
		// stream without changing the file. It is implementation defined
		// which (if any) mode changes are allowed. I currently define
		// the set to be empty.
		fclose(stream);
		return NULL;
	}
	close(stream->fd);
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
	_delete_file(f);
	return close_result;
}

void clearerr(FILE *stream) { stream->error = 0; }

int feof(FILE *stream) { return stream->eof && stream->buffer_length == 0; }

int ferror(FILE *stream) { return stream->error != 0; }

int fileno(FILE *stream) { return stream->fd; }

int fseek(FILE *stream, long offset, int whence) {
	clear_buffer(stream);

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

long ftell(FILE *stream) { return stream->offset; }

int fseeko(FILE *stream, off_t offset, int whence) {
	return fseek(stream, offset, whence);
}

off_t ftello(FILE *stream) { return ftell(stream); }

int getc(FILE *f) {
	if (feof(f))
		return EOF;
	unsigned char c;
	int n = fread(&c, 1, 1, f);
	if (n != 1) {
		f->error = n;
		return EOF;
	}
	return c;
}

int fgetc(FILE *f) { return getc(f); }

int getchar(void) { return getc(stdin); }

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

int fputc(int c, FILE *f) { return putc(c, f); }

int putchar(int c) { return putc(c, stdout); }

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

void setlinebuf(FILE *stream) { setvbuf(stream, NULL, _IOLBF, BUFSIZ); }

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	stream->buffer_data = buf;
	stream->buffer_size = size != 0 ? size : BUFSIZ;
	stream->buffer_mode = mode;
	return 0;
}

void __nc_f_init(void) {
	list_init(&__all_files);
	stdin = fdopen(0, "r");
	setvbuf(stdin, NULL, _IOLBF, 0);
	stdout = fdopen(1, "w");
	setvbuf(stdout, NULL, _IOLBF, 0);
	stderr = fdopen(2, "w");
	setvbuf(stderr, NULL, _IONBF, 0);
}

void __nc_f_fini(void) {
	list_for_each_safe (&__all_files) {
		struct _FILE *f = container_of(struct _FILE, files_node, it);
		fclose(f);
	}
}
