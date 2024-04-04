#pragma once
#ifndef NG_RINGBUF_H
#define NG_RINGBUF_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct ringbuf {
	char *data;

	size_t size;
	size_t len;
	size_t head;
};

struct ringbuf *ring_new(size_t size);
void ring_emplace(struct ringbuf *ring, size_t size);
void ring_emplace_with_buffer(struct ringbuf *ring, size_t size, void *buffer);
void ring_free(struct ringbuf *ring);
size_t ring_write(struct ringbuf *, const void *data, size_t len);
size_t ring_read(struct ringbuf *, void *data, size_t len);
size_t ring_data_len(struct ringbuf *r);

END_DECLS

#endif // NG_RINGBUF_H
