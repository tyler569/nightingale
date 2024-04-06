#include <stdlib.h>

struct ringbuf {
	char *buf;
	size_t size;
	size_t head;
	size_t tail;
};

size_t ringbuf_write(struct ringbuf *rb, const char *buf, size_t count) {
	size_t written = 0;
	while (count > 0) {
		if (rb->head == rb->tail) {
			return written;
		}
		rb->buf[rb->head] = *buf++;
		rb->head = (rb->head + 1) % rb->size;
		count--;
		written++;
	}
	return written;
}

size_t ringbuf_read(struct ringbuf *rb, char *buf, size_t count) {
	size_t read = 0;
	while (count > 0) {
		if (rb->head == rb->tail) {
			return read;
		}
		*buf++ = rb->buf[rb->tail];
		rb->tail = (rb->tail + 1) % rb->size;
		count--;
		read++;
	}
	return read;
}