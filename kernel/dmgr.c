#include <basic.h>
#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/mutex.h>
#include <ng/string.h>
#include <stdlib.h>

const int dmgr_initial = 16;

void dmgr_init(struct dmgr *d) {
    d->cap = dmgr_initial;
    d->data = zmalloc(sizeof(void *) * dmgr_initial);
    KMUTEX_INIT_LIVE(&d->lock);
}

static int _internal_dmgr_expand(struct dmgr *d) {
    void **new_data = zrealloc(d->data, d->cap * sizeof(void *) * 3 / 2);
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
    mutex_await(&d->lock);
    for (int i = 0;; i++) {
        if (i > d->cap) _internal_dmgr_expand(d);
        if (!d->data[i]) {
            d->data[i] = ptr;
            mutex_unlock(&d->lock);
            return i;
        }
    }
}

void *dmgr_get(struct dmgr *d, int handle) {
    if (handle > d->cap) return NULL;
    return d->data[handle];
}

void *dmgr_set(struct dmgr *d, int off, void *ptr) {
    mutex_await(&d->lock);
    d->data[off] = ptr;
    mutex_unlock(&d->lock);
    return ptr;
}

void *dmgr_drop(struct dmgr *d, int handle) {
    mutex_await(&d->lock);
    void *v = d->data[handle];
    d->data[handle] = 0;
    mutex_unlock(&d->lock);
    return v;
}

void dmgr_foreach(struct dmgr *d, void (*func)(void *)) {
    mutex_await(&d->lock);
    for (int i = 0; i < d->cap; i++) {
        void *val = d->data[i];
        if (val) {
            // printf("%i ", i);
            func(val);
        }
    }
    mutex_unlock(&d->lock);
}

void dmgr_foreachl(struct dmgr *d, void (*func)(void *, long), long v) {
    mutex_await(&d->lock);
    for (int i = 0; i < d->cap; i++) {
        void *val = d->data[i];
        if (val) func(val, v);
    }
    mutex_unlock(&d->lock);
}

void dmgr_foreachp(struct dmgr *d, void (*func)(void *, void *), void *p) {
    mutex_await(&d->lock);
    for (int i = 0; i < d->cap; i++) {
        void *val = d->data[i];
        if (val) func(val, p);
    }
    mutex_unlock(&d->lock);
}

void dmgr_copy(struct dmgr *child, struct dmgr *parent) {
    mutex_await(&parent->lock);
    memcpy(child, parent, sizeof(struct dmgr));
    child->data = malloc(parent->cap * sizeof(void *));
    memcpy(child->data, parent->data, parent->cap * sizeof(void *));
    mutex_unlock(&parent->lock);
    mutex_unlock(&child->lock); // memcpy picked this up
}

void dmgr_free(struct dmgr *d) {
    // TODO: should this do anything else?
    // maybe you just have to guarantee there's nothing left in a dmgr
    // before you destroy it?
    free(d->data);
}
