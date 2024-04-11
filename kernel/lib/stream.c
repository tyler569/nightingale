#include "stream.h"
#include "stream_buffer.h"

ssize_t buffer_stream_write(struct stream *, const void *data, size_t size);
ssize_t buffer_stream_read(struct stream *, void *data, size_t size);

struct stream_vtbl buffer_stream_vtbl = {
	.write = buffer_stream_write,
	.read = buffer_stream_read,
};

ssize_t serial_stream_write(struct stream *, const void *data, size_t size);

struct stream_vtbl serial_stream_vtbl = {
	.write = serial_stream_write,
};

FILE *w_stdout = &(FILE) {
	.vtbl = &serial_stream_vtbl,
	.fd = 1,
	.path = "stdout",
};

ssize_t buffer_stream_write(struct stream *s, const void *data, size_t size) {
	return stream_buffer_push(&s->write_buffer, data, size);
}

ssize_t buffer_stream_read(struct stream *s, void *data, size_t size) {
	return stream_buffer_pop(&s->write_buffer, data, size);
}

FILE buffer_stream(void *data, size_t size) {
	return (FILE) {
		.vtbl = &buffer_stream_vtbl,
		.write_buffer = {
			.data = data,
			.size = size,
		},
	};
}

#include <ng/serial.h>
#include <ng/x86/uart.h>

ssize_t serial_stream_write(struct stream *, const void *data, size_t size) {
	serial_write_str(x86_com[0], data, size);
	return size;
}