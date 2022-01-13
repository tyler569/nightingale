#pragma once
#ifndef NG_RINGBUF_H
#define NG_RINGBUF_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

struct ringbuf {
    char *data;

    size_t size;
    size_t len;
    size_t head;
};

struct ringbuf *new_ring(size_t size);
void emplace_ring(struct ringbuf *ring, size_t size);
void emplace_ring_with_buffer(struct ringbuf *ring, size_t size, void *buffer);
void free_ring(struct ringbuf *);
size_t ring_write(struct ringbuf *, const void *data, size_t len);
size_t ring_read(struct ringbuf *, void *data, size_t len);

#define ring_emplace emplace_ring // TODO swap

#endif // NG_RINGBUF_H
