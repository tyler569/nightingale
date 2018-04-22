
#pragma once
#ifndef NIGHTINGALE_RINGBUF_H
#define NIGHTINGALE_RINGBUF_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>

struct ringbuf {
    void *data;

    size_t size;
    size_t len;
    size_t head
};

struct ringbuf *new_ring(size_t size);
void emplace_ring(struct ringbuf *ring, size_t size);
size_t ring_write(struct ringbuf *ring, const void *data, size_t len);
size_t ring_read(struct ringbuf *ring, void *data, size_t len);

#endif

