#include <assert.h>
#include <ng/mem.h>
#include <ng/pmm.h>

int pm_incref(phys_addr_t pma) { return 1; }
int pm_decref(phys_addr_t pma) { return 1; }

phys_addr_t pm_alloc() { return alloc_page(); }

phys_addr_t pm_alloc_contiguous(size_t) {
	assert("Unsupported at this time");
	unreachable();
}

void pm_free(phys_addr_t pma) { free_page(pma); }
