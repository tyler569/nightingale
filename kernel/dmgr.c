
#include <basic.h>
#include <ng/debug.h>
#include <ng/malloc.h>
#include <ng/string.h>
#include <ng/dmgr.h>
#include <ng/mutex.h>

const int dmgr_initial = 16;

void dmgr_init(struct dmgr *d) {
        d->cap = dmgr_initial;
        d->data = malloc(sizeof(void *) * dmgr_initial);
        d->lock = 0;
}

static int _internal_dmgr_expand(struct dmgr *d) {
        void **new_data = realloc(d->data, d->cap * 2 * sizeof(void *));
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
        await_mutex(&d->lock);
        for (int i=0; ; i++) {
                if (i > d->cap)  _internal_dmgr_expand(d);
                if (!d->data[i])  {
                        d->data[i] = ptr;
                        release_mutex(&d->lock);
                        return i;
                }
        }
}

void *dmgr_get(struct dmgr *d, int handle) {
        if (handle > d->cap)  return NULL;
        return d->data[handle];
}

void *dmgr_set(struct dmgr *d, int off, void *ptr) {
        await_mutex(&d->lock);
        d->data[off] = ptr;
        release_mutex(&d->lock);
        return ptr;
}

void *dmgr_drop(struct dmgr *d, int handle) {
        await_mutex(&d->lock);
        void *v = d->data[handle];
        d->data[handle] = 0;
        release_mutex(&d->lock);
        return v;
}

void dmgr_foreach(struct dmgr *d, void (*func)(void *)) {
        await_mutex(&d->lock);
        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val);
        }
        release_mutex(&d->lock);
}

void dmgr_foreachl(struct dmgr *d, void (*func)(void *, long), long v) {
        await_mutex(&d->lock);
        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val, v);
        }
        release_mutex(&d->lock);
}

void dmgr_foreachp(struct dmgr *d, void (*func)(void *, void *), void *p) {
        await_mutex(&d->lock);
        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val, p);
        }
        release_mutex(&d->lock);
}

void dmgr_copy(struct dmgr *child, struct dmgr *parent) {
        await_mutex(&parent->lock);
        memcpy(child, parent, sizeof(struct dmgr));
        child->data = malloc(parent->cap * sizeof(void *));
        memcpy(child->data, parent->data, parent->cap * sizeof(void *));
        release_mutex(&parent->lock);
        release_mutex(&child->lock); // memcpy picked this up
}

void dmgr_free(struct dmgr *d) {
        // TODO: should this do anything else?
        // maybe you just have to guarantee there's nothing left in a dmgr
        // before you destroy it?
        free(d->data);
}

