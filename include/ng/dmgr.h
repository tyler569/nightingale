#pragma once

#include <ng/sync.h>
#include <sys/cdefs.h>

struct dmgr {
	int cap;
	void **data;
	mutex_t lock;
};

BEGIN_DECLS

void dmgr_init(struct dmgr *d);
int dmgr_insert(struct dmgr *d, void *ptr);
void *dmgr_get(struct dmgr *d, int handle);

void *dmgr_set(struct dmgr *d, int handle, void *newptr);
void *dmgr_drop(struct dmgr *d, int handle);

END_DECLS

