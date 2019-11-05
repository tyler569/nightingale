
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/malloc.h>
#include <ng/string.h>
#include <ng/dmgr.h>

void dmgr_init(struct dmgr *d) {
        DEBUG_PRINTF("dmgr_init(d)\n");
        d->cap = 16;
        d->len = 0;
        d->full = 0;
        d->first_free = -1;
        d->data = malloc(sizeof(struct dmgr_element) * 16);
}

static int _internal_dmgr_expand(struct dmgr *d) {
        DEBUG_PRINTF("dmgr_expand(d) (len = %i)\n", d->len);
        struct dmgr_element *new_data =
            realloc(d->data, d->len * 2 * sizeof(struct dmgr_element));
        if (new_data) {
                d->data = new_data;
                d->cap *= 2;
                return d->cap;
        } else {
                // panic();
                return -1;
        }
}

int dmgr_insert(struct dmgr *d, void *ptr) {
        DEBUG_PRINTF("dmgr_insert(d, %p)\n", ptr);
        int insert_at;
        if (d->full < d->len) {
                insert_at = d->first_free;
                int next_free = d->data[insert_at].handle;
                d->first_free = next_free;
        } else {
                insert_at = d->len;
                if (d->cap == d->len)
                        _internal_dmgr_expand(d);
                d->len += 1;
        }
        d->data[insert_at].handle = insert_at;
        d->data[insert_at].pointer = ptr;
        d->full += 1;
        return insert_at;
}

void *dmgr_get(struct dmgr *d, int handle) {
        DEBUG_PRINTF("dmgr_get(d, %i)\n", handle);
        if (handle > d->cap) {
                return NULL;
        }
        if (handle == d->data[handle].handle) {
                return d->data[handle].pointer;
        } else {
                return NULL;
        }
}

#if 0
// This needs more thought!
// what if you wanna clobber the free list?
// what should ->len do?
int dmgr_set(struct dmgr *d, size_t off, void *ptr) {
        DEBUG_PRINTF("dmgr_set(d, %zu, %p)\n", off, ptr);

        while (d->cap < off) {
                _internal_dmgr_expand(d);
        }

        d->data[off].handle = insert_at;
        d->data[off].pointer = ptr;
        d->full += 1;
}
#endif

void *dmgr_drop(struct dmgr *d, int handle) {
        DEBUG_PRINTF("dmgr_drop(d, %i)\n", handle);
        // returns the pointer so you can free it / drop it.
        if (handle > d->cap) {
                return NULL;
        }
        if (handle == d->data[handle].handle) {
                d->data[handle].handle = d->first_free;
                d->first_free = handle;
                d->full -= 1;
                return d->data[handle].pointer;
        } else {
                return NULL;
        }
}

void dmgr_foreach(struct dmgr *d, void (*func)(void *)) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = dmgr_get(d, i);
                if (val)  func(val);
        }
}

void dmgr_foreachl(struct dmgr *d, void (*func)(void *, long), long v) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = dmgr_get(d, i);
                if (val)  func(val, v);
        }
}

void dmgr_foreachp(struct dmgr *d, void (*func)(void *, void *), void *p) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = dmgr_get(d, i);
                if (val)  func(val, p);
        }
}

void dmgr_copy(struct dmgr *child, struct dmgr *parent) {
        memcpy(child, parent, sizeof(struct dmgr));
        child->data = malloc(parent->cap * sizeof(struct dmgr_element));
        memcpy(child->data, parent->data,
                parent->cap * sizeof(struct dmgr_element));
        return;
}

// Change a value at a valid dmgr location
void *dmgr_swap(struct dmgr *d, int handle, void *new_ptr) {
        DEBUG_PRINTF("dmgr_swap(d, %i, %p)\n", handle, new_ptr);
        if (handle > d->cap) {
                return NULL;
        }
        if (handle == d->data[handle].handle) {
                d->data[handle].pointer = new_ptr;
                return new_ptr;
        } else {
                return NULL;
        }
}

void dmgr_free(struct dmgr *d) {
        // TODO: should this do anything else?
        // maybe you just have to guarantee there's nothing left in a dmgr
        // before you destroy it?
        free(d->data);
}

