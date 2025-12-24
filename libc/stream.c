#include <assert.h>
#include <stream.h>
#include <string.h>
#ifndef __kernel__
#include <unistd.h>
#endif

// streambuf implementation

bool streambuf_empty(struct streambuf *b) {
	return b->length == 0;
}

bool streambuf_full(struct streambuf *b) {
	return b->length == b->capacity;
}

void streambuf_clear(struct streambuf *b) {
	b->length = 0;
}

size_t streambuf_contiguous_space(struct streambuf *b, void **pos) {
	if (pos) {
		*pos = b->data + b->length;
	}

	return b->capacity - b->length;
}

ssize_t streambuf_push(struct streambuf *b, const void *data, size_t size) {
	// Write what we can
	size_t available = b->capacity - b->length;
	size_t to_write = MIN(size, available);
	memcpy(b->data + b->length, data, to_write);
	b->length += to_write;
	return to_write;
}

ssize_t streambuf_pop(struct streambuf *b, void *data, size_t size) {
	// Read what we can
	size_t to_read = MIN(size, b->length);
	memcpy(data, b->data, to_read);

	// Move remaining data to the front
	size_t remaining = b->length - to_read;
	if (remaining > 0) {
		memmove(b->data, b->data + to_read, remaining);
	}
	b->length = remaining;

	return to_read;
}

bool streambuf_memchr(struct streambuf *b, int c, size_t *offset) {
	assert(offset);

	// Search from the beginning
	const char *found = memchr(b->data, c, b->length);
	if (found) {
		*offset = found - (const char *)b->data;
		return true;
	}
	return false;
}

// stream implementation

ssize_t buffer_stream_write(struct stream *, const void *data, size_t size);
ssize_t buffer_stream_read(struct stream *, void *data, size_t size);

struct stream_vtbl buffer_stream_vtbl = {
	.write = buffer_stream_write,
	.read = buffer_stream_read,
};

ssize_t sprintf_stream_write(struct stream *s, const void *data, size_t size);

struct stream_vtbl sprintf_stream_vtbl = {
	.write = sprintf_stream_write,
};

ssize_t serial_stream_write(struct stream *, const void *data, size_t size);

struct stream_vtbl serial_stream_vtbl = {
	.write = serial_stream_write,
};

struct stream *w_stdout = &(struct stream) {
	.vtbl = &serial_stream_vtbl,
	.fd = 1,
};

ssize_t buffer_stream_write(struct stream *s, const void *data, size_t size) {
	return streambuf_push(&s->buf, data, size);
}

ssize_t buffer_stream_read(struct stream *s, void *data, size_t size) {
	return streambuf_pop(&s->buf, data, size);
}

ssize_t sprintf_stream_write(struct stream *s, const void *data, size_t size) {
	memcpy(s->context + s->offset, data, size);
	s->offset += size;
	return size;
}

struct stream sprintf_stream(void *buffer) {
	return (struct stream) {
		.vtbl = &sprintf_stream_vtbl,
		.context = buffer,
	};
}

#ifdef __kernel__

#include <ng/serial.h>
#include <ng/x86/uart.h>

ssize_t serial_stream_write(struct stream *, const void *data, size_t size) {
	if (!*(const char *)data) {
		return 0;
	}
	serial_write_str(x86_com[0], data, size);
	return size;
}

#else

ssize_t serial_stream_write(struct stream *, const void *data, size_t size) {
	if (!*(const char *)data) {
		return 0;
	}
	return write(1, data, size);
}

#endif
