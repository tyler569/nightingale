#pragma once

#include <stddef.h>
#include <sys/types.h>

struct streambuf {
	void *data;
	size_t capacity;
	size_t length;
};

bool streambuf_empty(struct streambuf *);
bool streambuf_full(struct streambuf *);
void streambuf_clear(struct streambuf *);
ssize_t streambuf_push(struct streambuf *, const void *d, size_t s);
ssize_t streambuf_pop(struct streambuf *, void *d, size_t s);
bool streambuf_memchr(struct streambuf *, int c, size_t *offset);
size_t streambuf_contiguous_space(struct streambuf *b, void **pos);

struct stream;

#ifdef __kernel__
typedef struct stream FILE;
#endif

struct stream_vtbl {
	ssize_t (*write)(struct stream *stream, const void *data, size_t size);
	ssize_t (*read)(struct stream *stream, void *data, size_t size);
	void (*close)(struct stream *stream);
	void (*flush)(struct stream *stream);
};

enum stream_buffer_mode : char {
	STREAM_BUFFER_NONE,
	STREAM_BUFFER_LINE,
	STREAM_BUFFER_FULL,
};

struct stream {
	const struct stream_vtbl *vtbl;
	void *context;

	struct streambuf buf;

	off_t offset;
	int fd;

	enum stream_buffer_mode buffer_mode;
	bool error;
	bool eof;
	char unget_char;
};

#define F_WRITE(s, d, sz) (s)->vtbl->write((s), (d), (sz))
#define F_READ(s, d, sz) (s)->vtbl->read((s), (d), (sz))
#define F_CLOSE(s) (s)->vtbl->close((s))
#define F_FLUSH(s) (s)->vtbl->flush((s))

struct stream buffer_stream(void *data, size_t size);
struct stream sprintf_stream(void *buffer);

extern struct stream *w_stdout;
