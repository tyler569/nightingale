#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct page {
	uint32_t refcount;
};

enum {
	PM_NOMEM = 0,
	PM_LEAK = 1,
	PM_REF_BASE = 2,
	PM_REF_ZERO = PM_REF_BASE,
};

void pm_init();
// int pm_getref(phys_addr_t pma);
int pm_incref(phys_addr_t pma);
int pm_decref(phys_addr_t pma);
phys_addr_t pm_alloc_contiguous(size_t n_pages);
phys_addr_t pm_alloc();
void pm_free(phys_addr_t);
void pm_set(phys_addr_t base, phys_addr_t top, uint32_t set_to);

struct open_file;
int pm_avail();

END_DECLS
