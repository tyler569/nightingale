
#include <ng/basic.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/panic.h>
#include <ng/malloc.h>
#include <ng/string.h>
#include <ds/ringbuf.h>

struct ringbuf *new_ring(size_t size) {
    struct ringbuf *ring = malloc(sizeof(struct ringbuf));
    ring->data = malloc(size);
    ring->size = size;
    ring->len = 0;
    ring->head = 0;
    return ring;
}

void emplace_ring(struct ringbuf *ring, size_t size) {
    ring->data = malloc(size);
    ring->size = size;
    ring->len = 0;
    ring->head = 0;
}

size_t ring_write(struct ringbuf *r, const void *data, size_t len) {
    if (r->head > r->len) {
        size_t count = min(len, r->size - r->head);
        memcpy(r->data + r->head, data, count);
        r->head += count;
        r->len += count;
        r->head %= r->size; // 0 if at ->size

        if (count < len) {
            count += ring_write(r, (const char*)data + count, len - count);
        }
        return count;
    }

    if (r->head <= r->len) {
        size_t count = min(len, r->size - r->len);
        memcpy(r->data + r->head, data, count);
        r->head += count;
        r->len += count;
        r->head %= r->size; // shouldn't be needed
        return count;
    }

    panic("No condition matched, did we race the ring?\n");
}

size_t ring_read(struct ringbuf *r, void *data, size_t len) {
    if (r->len == 0)
        return 0;

    if (r->head >= r->len) {
        size_t count = min(len, r->len);
        memcpy(data, r->data + r->head - r->len, count);
        r->len -= count;
        return count;
    }

    if (r->head < r->len) {
        size_t count = min(len, r->len - r->head);
        memcpy(data, r->data + r->size - r->len + r->head, count);
        r->len -= count;
        if (count < len) {
            count += ring_read(r, (char*)data + count, len - count);
        }
        return count;
    }

    panic("No condition matched, did we race the ring?\n");
}

