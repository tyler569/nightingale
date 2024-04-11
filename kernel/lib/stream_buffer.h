#pragma once

#include <stddef.h>
#include <sys/types.h>

struct stream_buffer {
	void *data;
	size_t size;
	size_t head;
	size_t tail;
};

bool stream_buffer_empty(struct stream_buffer *);
bool stream_buffer_full(struct stream_buffer *);
ssize_t stream_buffer_push(struct stream_buffer *, const void *d, size_t s);
ssize_t stream_buffer_pop(struct stream_buffer *, void *d, size_t s);