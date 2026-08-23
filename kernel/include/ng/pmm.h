#pragma once

#include <ng/mman.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct page {
	uint32_t refcount; // top bit is "page exists", 31 bit refcount.
};

enum {
	PM_NOMEM = 0,
	PM_LEAK = 1,
	PM_REF_BASE = 2,
	PM_REF_ZERO = PM_REF_BASE,
};

void pm_incref(phys_addr_t pma);
void pm_decref(phys_addr_t pma);
phys_addr_t pm_alloc();
void pm_free(phys_addr_t);

int pm_avail();

END_DECLS
