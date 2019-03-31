
#include <basic.h>
#include <debug.h>
#include <malloc.h>
#include "dmgr.h"

void dmgr_init(struct dmgr* d) {
    DEBUG_PRINTF("dmgr_init(d)\n");
    d->cap = 16;
    d->len = 0;
    d->full = 0;
    d->first_free = -1;
    d->data = malloc(sizeof(struct dmgr_element) * 16);
}

int _internal_dmgr_expand(struct dmgr* d) {
    DEBUG_PRINTF("dmgr_expand(d) (len = %i)\n", d->len);
    struct dmgr_element* new_data =
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

int dmgr_insert(struct dmgr* d, void* ptr) {
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

void* dmgr_get(struct dmgr* d, int handle) {
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

void* dmgr_drop(struct dmgr* d, int handle) {
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

