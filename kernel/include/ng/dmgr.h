#pragma once

#include <ng/sync.h>
#include <sys/cdefs.h>

struct dmgr {
	int cap;
	void **data;
	spinlock_t lock;
};

BEGIN_DECLS

int dmgr_insert(struct dmgr *, void *ptr);
void *dmgr_get(struct dmgr *, int handle);
void *dmgr_set(struct dmgr *, int handle, void *newptr);
void *dmgr_drop(struct dmgr *, int handle);
void dmgr_clone(struct dmgr *child, struct dmgr *parent);

END_DECLS
