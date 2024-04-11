#pragma once

#include <stddef.h>
#include <sys/types.h>

struct stream_ring {
	void *data;
	size_t size;
	size_t head;
	size_t tail;
};

bool stream_ring_empty(struct stream_ring *);
bool stream_ring_full(struct stream_ring *);
ssize_t stream_ring_push(struct stream_ring *, const void *d, size_t s);
ssize_t stream_ring_pop(struct stream_ring *, void *d, size_t s);

bool stream_ring_memchr(struct stream_ring *, int c, size_t *offset);
size_t stream_ring_contiguous_space(struct stream_ring *b, void **pos);
