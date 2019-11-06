
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/malloc.h>
#include <ng/string.h>
#include <ng/dmgr.h>

const int dmgr_initial = 16;

void dmgr_init(struct dmgr *d) {
        DEBUG_PRINTF("dmgr_init(d)\n");
        d->cap = dmgr_initial;
        d->data = malloc(sizeof(void *) * dmgr_initial);;
}

static int _internal_dmgr_expand(struct dmgr *d) {
        DEBUG_PRINTF("dmgr_expand(d) (len = %i)\n", d->cap);
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
        DEBUG_PRINTF("dmgr_insert(d, %p)\n", ptr);
        for (int i=0; ; i++) {
                if (i > d->cap)  _internal_dmgr_expand(d);
                if (!d->data[i])  {
                        d->data[i] = ptr;
                        return i;
                }
        }
}

void *dmgr_get(struct dmgr *d, int handle) {
        DEBUG_PRINTF("dmgr_get(d, %i)\n", handle);
        return d->data[handle];
}

void *dmgr_set(struct dmgr *d, int off, void *ptr) {
        DEBUG_PRINTF("dmgr_set(d, %zu, %p)\n", off, ptr);
        d->data[off] = ptr;
        return ptr;
}

void *dmgr_drop(struct dmgr *d, int handle) {
        DEBUG_PRINTF("dmgr_drop(d, %i)\n", handle);
        void *v = d->data[handle];
        d->data[handle] = 0;
        return v;
}

void dmgr_foreach(struct dmgr *d, void (*func)(void *)) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val);
        }
}

void dmgr_foreachl(struct dmgr *d, void (*func)(void *, long), long v) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val, v);
        }
}

void dmgr_foreachp(struct dmgr *d, void (*func)(void *, void *), void *p) {
        DEBUG_PRINTF("dmgr_foreach(d, func)\n");

        for (int i=0; i<d->cap; i++) {
                void *val = d->data[i];
                if (val)  func(val, p);
        }
}

void dmgr_copy(struct dmgr *child, struct dmgr *parent) {
        memcpy(child, parent, sizeof(struct dmgr));
        child->data = malloc(parent->cap * sizeof(void *));
        memcpy(child->data, parent->data, parent->cap * sizeof(void *));
        return;
}

void dmgr_free(struct dmgr *d) {
        // TODO: should this do anything else?
        // maybe you just have to guarantee there's nothing left in a dmgr
        // before you destroy it?
        free(d->data);
}

