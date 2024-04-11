#include <ng/common.h>
#include <stream_ring.h>
#include <string.h>

bool stream_ring_empty(struct stream_ring *b) { return b->head == b->tail; }

bool stream_ring_full(struct stream_ring *b) {
	return b->tail == (b->head + 1) % b->size;
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
