#include <stream.h>
#include <stream_ring.h>
#include <string.h>

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
	return stream_ring_push(&s->ring, data, size);
}

ssize_t buffer_stream_read(struct stream *s, void *data, size_t size) {
	return stream_ring_pop(&s->ring, data, size);
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

#endif
