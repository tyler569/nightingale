#include <assert.h>
#include <stream_ring.h>
#include <string.h>

bool stream_ring_empty(struct stream_ring *b) { return b->head == b->tail; }

bool stream_ring_full(struct stream_ring *b) {
	return b->tail == (b->head + 1) % b->size;
}

void stream_ring_clear(struct stream_ring *b) { b->head = b->tail = 0; }

size_t stream_ring_contiguous_space(struct stream_ring *b, void **pos) {
	if (pos) {
		*pos = b->data + b->head;
	}

	if (b->head >= b->tail) {
		return b->size - b->head;
	}
	return b->tail - b->head - 1;
}

ssize_t stream_ring_push(struct stream_ring *b, const void *data, size_t size) {
	ssize_t written = 0;
	ptrdiff_t copy_len;
	if (b->head >= b->tail) {
		copy_len = MIN(size, b->size - b->head);
		memcpy(b->data + b->head, data, copy_len);
		size -= copy_len;
		b->head += copy_len;
		written += copy_len;
		if (b->head == b->size) {
			b->head = 0;
		}
	}
	copy_len = MIN(size, b->tail - b->head - 1);
	memcpy(b->data + b->head, data + written, copy_len);
	b->head += copy_len;
	written += copy_len;
	return written;
}

ssize_t stream_ring_pop(struct stream_ring *b, void *data, size_t size) {
	ssize_t read = 0;
	ptrdiff_t copy_len;
	if (b->head < b->tail) {
		copy_len = MIN(size, b->size - b->tail);
		memcpy(data, b->data + b->tail, copy_len);
		size -= copy_len;
		b->tail += copy_len;
		read += copy_len;
		if (b->tail == b->size) {
			b->tail = 0;
		}
	}
	copy_len = MIN(size, b->head - b->tail);
	memcpy(data + read, b->data + b->tail, copy_len);
	b->tail += copy_len;
	read += copy_len;
	return read;
}

bool stream_ring_memchr(struct stream_ring *b, int c, size_t *offset) {
	assert(offset);

	size_t i = b->tail;
	size_t n = 0;
	while (i != b->head) {
		if (*((char *)b->data + i) == c) {
			*offset = n;
			return true;
		}
		i = (i + 1) % b->size;
		n++;
	}
	return false;
}
