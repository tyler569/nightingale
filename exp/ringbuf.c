
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * A testbed in normal *nix userspace for a new
 * ring buffer for nightingale
 *
 */

#ifndef NDEBUG
#define trace_printf printf
#else
#define trace_printf(...) 
#endif

#define min(x, y) ((x) > (y)) ? (y) : (x)
#define max(x, y) ((x) < (y)) ? (y) : (x)

struct ringbuf {
    void *data;

    size_t size;
    size_t len;
    size_t head;
};

size_t ring_write(struct ringbuf *r, void *data, size_t len) {
    if (r->head > r->len) {
        size_t count = min(len, r->size - r->head);
        memcpy(r->data + r->head, data, count);
        r->head += count;
        r->len += count;
        r->head %= r->size; // 0 if at ->size
        if (count < len) {
            count += ring_write(r, data + count, len - count);
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
}

size_t ring_read(struct ringbuf *r, void *data, size_t len) {
    if (r->head > r->len) {
        size_t count = min(len, r->len);
        memcpy(data, r->data + r->head - r->len, count);
        r->len -= count;
        return count;
    }

    if (r->head == r->len) {
        size_t count = min(len, r->len);
        memcpy(data, r->data, count);
        r->len -= count;
        return count;
    }

    if (r->head < r->len) {
        size_t count = min(len, r->len - r->head);
        memcpy(data, r->data + r->size - r->len + r->head, count);
        r->len -= count;
        if (count < len) {
            count += ring_read(r, data + count, len - count);
        }
        return count;
    }
}

void ring_debug(struct ringbuf *ring) {
    printf("  ring->len: %li   ring->head: %li\n", ring->len, ring->head);
}

struct ringbuf *new_ring(size_t size) {
    struct ringbuf *ring = malloc(sizeof(struct ringbuf));
    ring->data = malloc(size);
    ring->size = size;
    ring->len = 0;
    ring->head = 0;
}

#if 0 // trying again

struct ringbuf {
    size_t len;
//    size_t spill_threshold; // if we're X from the end of the region, just move on
    size_t count;
    void *head;
    void *tail;
    void *region;
    void *end;
};

void ring_debug(struct ringbuf *ring) {
    printf("ring @ %#lx:\n", (uintptr_t)ring);
    printf(" region @ %#lx\n", (uintptr_t)ring->region);
    printf(" count  = %#lx\n", ring->count);
    printf("    head: +%#lx\n", ring->head - ring->region);
    printf("    tail: +%#lx\n", ring->tail - ring->region);
}

ssize_t ring_write(struct ringbuf *ring, void *data, size_t count);
ssize_t ring_read(struct ringbuf *ring, void *data, size_t count);
struct ringbuf *new_ring(size_t len);
void del_ring(struct ringbuf *ring);

struct ringbuf *new_ring(size_t len) {
    struct ringbuf *ring = malloc(sizeof(struct ringbuf));

    ring->region = malloc(len);
    ring->end = ring->region + len;
    ring->head = ring->region;
    ring->tail = ring->region;
    ring->spill_threshold = 0; // later
    ring->count = 0;

    return ring;
}

void del_ring(struct ringbuf *ring) {
    free(ring->region);
    free(ring);
}

ssize_t ring_write(struct ringbuf *ring, void *data, size_t count) {
    if (ring->head == ring->tail && ring->count == 0) {
        trace_printf("1 ");
        // ring is currently empty;
        if (count <= ring->head - ring->end) {
            // we have enough space to just write it no problems
            trace_printf("2 ");
            memcpy(ring->head, data, count);
            ring->head += count;
            ring->count += count;
            return count;
        } else {
            trace_printf("3 ");
            size_t written = ring->len;
            memcpy(ring->head, data, written);
            // ring->head; // doesn't move, still @tail @0
            ring->count += written;
            return written;
        }
    } else if (ring->head > ring->tail) {
        trace_printf("4 ");
        if (ring->head + count < ring->end) {
            trace_printf("5 ");
            // we have enough space to just write it no problems
            memcpy(ring->head, data, count);
            ring->head += count;
            ring->count += count;
            return count;
        } else {
            trace_printf("6 ");
            size_t written = ring->end - ring->head;
            memcpy(ring->head, data, written);
            if (ring->tail == ring->region) {
                trace_printf("7 ");
                // ring is full
                ring->head += written;
                ring->count += written;
                return written;
            } else {
                trace_printf("8 ");
                ring->head = ring->region;
                ring->count += written;
                return written + ring_write(ring, data + written, count - written);
            }
        }
    } else {
        trace_printf("9 ");
        if (ring->count == ring->len) {
            trace_printf("10 ");
            // ring is full
            // block or error?
            // maybe make that configurable
            printf("wrote to full ring\n");
            return 0;
        } else if (ring->tail - ring->head > count) {
            trace_printf("11 ");
            size_t written = ring->tail - ring->head;
            memcpy(ring->head, data, written);
            ring->head += written;
            ring->count += written;
            return ring->tail - ring->head;
            // ring is now full
        } else {
            trace_printf("12 ");
            // we have room
            memcpy(ring->head, data, count);
            ring->head += count;
            ring->count += count;
            return count;
        }
    }
}

/*
size_t ring_write(struct ringbuf *ring, void *data, size_t count) {
    if (ring->head)
}
*/

ssize_t ring_read(struct ringbuf *ring, void *data, size_t count) {
    if (ring->head == ring->tail && ring->count == 0) {
        printf("read from empty ring\n");
        return 0;
    } else if (ring->head > ring->tail) {
        ssize_t read = (ring->head - ring->tail) > count ? count : (ring->head - ring->tail);
        memcpy(data, ring->tail, read);
        ring->tail += read;
        ring->count -= read;
        return read;
    } else {
        ssize_t read = (ring->end - ring->tail) > count ? count : (ring->end - ring->tail);
        memcpy(data, ring->tail, read);
        count -= read;
        if (read) {
            ring->tail = ring->region;
            ring->count -= read;
            return read + ring_read(ring, data + read, count);
        } else {
            ring->tail += read;
            ring->count -= read;
            return read;
        }
    }
}

#endif

int main() {

    char data[20] = {0};
    ssize_t r;

    struct ringbuf *test = new_ring(20);
    ring_debug(test);

    r = ring_write(test, "1", 2);
    ring_debug(test);

    r = ring_read(test, data, 2);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);
    
    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    r = ring_write(test, "test test test", 16);
    ring_debug(test);

    r = ring_read(test, data, 20);
    printf("data: %s\n", data);
    ring_debug(test);

    return 0;
}

