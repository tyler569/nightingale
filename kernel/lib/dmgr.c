#include <ng/debug.h>
#include <ng/dmgr.h>
#include <ng/string.h>
#include <ng/sync.h>
#include <stdlib.h>

#define DMGR_INITIAL_CAPACITY 16

static void dmgr_resize(struct dmgr *d, int new_cap) {
	void **new_data = zrealloc(d->data, new_cap * sizeof(void *));
	if (new_data) {
		d->data = new_data;
		d->cap = new_cap;
	} else {
		panic("dmgr_expand_internal: realloc failed");
	}
}

static void dmgr_expand(struct dmgr *d) {
	size_t new_cap = MAX(d->cap * 3 / 2, DMGR_INITIAL_CAPACITY);
	dmgr_resize(d, new_cap);
}

static void dmgr_expand_to(struct dmgr *d, int cap) {
	size_t new_cap = MAX(d->cap, cap + 16);
	if (new_cap > d->cap)
		dmgr_resize(d, new_cap);
}

int dmgr_insert(struct dmgr *d, void *ptr) {
	spin_lock(&d->lock);
	int i = 0;

	for (;; i++) {
		if (i >= d->cap)
			dmgr_expand(d);
		if (!d->data[i]) {
			d->data[i] = ptr;
			break;
		}
	}

	spin_unlock(&d->lock);
	return i;
}

void *dmgr_get(struct dmgr *d, int handle) {
	if (handle > d->cap)
		return nullptr;

	return d->data[handle];
}

void *dmgr_set(struct dmgr *d, int handle, void *ptr) {
	if (handle >= d->cap)
		dmgr_expand_to(d, handle);

	spin_lock(&d->lock);
	void *current = d->data[handle];

	d->data[handle] = ptr;

	spin_unlock(&d->lock);
	return current;
}

void *dmgr_drop(struct dmgr *d, int handle) {
	if (handle > d->cap)
		return nullptr;

	spin_lock(&d->lock);

	void *v = d->data[handle];
	d->data[handle] = 0;

	spin_unlock(&d->lock);
	return v;
}

void dmgr_clone(struct dmgr *child, struct dmgr *parent) {
	spin_lock(&parent->lock);

	*child = (struct dmgr) {
		.cap = parent->cap,
		.data = nullptr,
		.lock = {},
	};

	child->data = malloc(parent->cap * sizeof(void *));
	memcpy(child->data, parent->data, parent->cap * sizeof(void *));

	spin_unlock(&parent->lock);
}

void dmgr_free(struct dmgr *d) { free(d->data); }

void dmgr_dump(struct dmgr *d) {
	spin_lock(&d->lock);

	printf("dmgr %p { .cap = %d, .data = %p }\n", (void *)d, d->cap,
		(void *)d->data);
	for (int i = 0; i < d->cap; i++) {
		if (d->data[i]) {
			printf("  %p: [%i] = %p\n", (void *)&d->data[i], i,
				(void *)d->data[i]);
		}
	}

	spin_unlock(&d->lock);
}
