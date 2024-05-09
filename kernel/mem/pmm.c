#include <assert.h>
#include <ng/fs.h>
#include <ng/pmm.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <ng/vmm.h>

static void pm_summary_imm();

static spin_lock_t pm_lock = { 0 };

#define NBASE (32 * PAGE_SIZE)

// page refcounts for bottom 64M of physical memory
uint8_t base_page_refcounts[NBASE] = { 0 };

void pm_init() {
	pm_incref(0x8000); // saved for AP initialization code
}

/*
 * -1: no such memory
 *  0: unused
 *  1+: used.
 */
int pm_refcount(phys_addr_t pma) {
	size_t offset = pma / PAGE_SIZE;
	if (offset >= NBASE)
		return -1;

	uint8_t value = base_page_refcounts[offset];
	if (value == PM_NOMEM) {
		return -1;
	} else if (value == PM_LEAK) {
		return 1;
	} else {
		return value - 2;
	}
}

int pm_incref(phys_addr_t pma) {
	size_t offset = pma / PAGE_SIZE;
	if (offset >= NBASE)
		return -1;

	uint8_t current = base_page_refcounts[offset];
	if (current < PM_REF_BASE) {
		// if the page is leaked or non-extant, just say it's still
		// in use and return.
		return 1;
	}

	spin_lock(&pm_lock);
	base_page_refcounts[offset] += 1;
	spin_unlock(&pm_lock);
	return base_page_refcounts[offset] - PM_REF_ZERO;
}

int pm_decref(phys_addr_t pma) {
	size_t offset = pma / PAGE_SIZE;
	if (offset >= NBASE)
		return -1;

	uint8_t current = base_page_refcounts[offset];

	if (current < PM_REF_BASE) {
		// if the page is leaked or non-extant, just say it's still
		// in use and return.
		return 1;
	}

	// but _do_ error if the refcount is already zero, that means there's
	// a double free somewhere probably.
	assert(current != PM_REF_ZERO);

	spin_lock(&pm_lock);
	base_page_refcounts[offset] -= 1;
	spin_unlock(&pm_lock);

	return base_page_refcounts[offset] - PM_REF_ZERO;
}

void pm_set(phys_addr_t base, phys_addr_t top, uint8_t set_to) {
	phys_addr_t rbase, rtop;
	size_t base_offset, top_offset;

	rbase = ROUND_DOWN(base, PAGE_SIZE);
	rtop = ROUND_UP(top, PAGE_SIZE);

	base_offset = rbase / PAGE_SIZE;
	top_offset = rtop / PAGE_SIZE;

	spin_lock(&pm_lock);
	for (size_t i = base_offset; i < top_offset; i++) {
		if (i >= NBASE)
			break;
		// map entries can overlap, don't reset something already claimed.
		if (base_page_refcounts[i] == 1)
			continue;
		base_page_refcounts[i] = set_to;
	}
	spin_unlock(&pm_lock);
}

phys_addr_t pm_alloc() {
	spin_lock(&pm_lock);
	for (size_t i = 0; i < NBASE; i++) {
		if (base_page_refcounts[i] == PM_REF_ZERO) {
			base_page_refcounts[i]++;
			spin_unlock(&pm_lock);
			return i * PAGE_SIZE;
		}
	}
	spin_unlock(&pm_lock);
	assert("no more physical pages" && 0);
	printf("WARNING: OOM\n");
	kill_process(running_process, 1);
	return 0;
}

phys_addr_t pm_alloc_contiguous(size_t n_pages) {
	spin_lock(&pm_lock);
	for (size_t i = 0; i < NBASE; i++) {
		if (base_page_refcounts[i] != PM_REF_ZERO)
			continue;
		if (i + n_pages > NBASE)
			break;

		bool not_found = false;
		for (size_t j = 0; j < n_pages; j++) {
			if (base_page_refcounts[i + j] != PM_REF_ZERO) {
				i += j;
				not_found = true;
				break;
			}
		}
		if (not_found)
			continue;

		for (size_t j = 0; j < n_pages; j++) {
			base_page_refcounts[i + j]++;
		}
		spin_unlock(&pm_lock);
		return i * PAGE_SIZE;
	}
	spin_unlock(&pm_lock);
	printf("WARNING: OOM\n");
	kill_process(running_process, 1);
	return 0;
}

void pm_free(phys_addr_t pma) { pm_decref(pma); }

static int disp(int refcount) {
	switch (refcount) {
	case PM_NOMEM:
		return 0;
	case PM_LEAK:
		return 1;
	case PM_REF_ZERO:
		return 2;
	default:
		return 3;
	}
}

static const char *type(int disp) {
	switch (disp) {
	case 0:
		return "nomem";
	case 1:
		return "leak";
	case 2:
		return "unused";
	case 3:
		return "in use";
	default:
		return "";
	}
}

void pm_summary(struct file *ofd, void *) {
	/* last:
	 * 0: PM_NOMEM
	 * 1: PM_LEAK
	 * 2: PM_REF_ZERO
	 * 3: any references
	 */
	uint8_t last = 0;
	size_t base = 0, i = 0;
	size_t inuse = 0, avail = 0, leak = 0;

	for (; i < NBASE; i++) {
		int ref = base_page_refcounts[i];
		int dsp = disp(ref);

		if (dsp == 1)
			leak += PAGE_SIZE;
		if (dsp == 2)
			avail += PAGE_SIZE;
		if (dsp == 3)
			inuse += PAGE_SIZE;
		if (dsp == last)
			continue;

		if (i > 0)
			proc_sprintf(
				ofd, "%010zx %010zx %s\n", base, i * PAGE_SIZE, type(last));
		base = i * PAGE_SIZE;
		last = dsp;
	}

	proc_sprintf(ofd, "%010zx %010zx %s\n", base, i * PAGE_SIZE, type(last));

	proc_sprintf(ofd, "available: %10zu (%10zx)\n", avail, avail);
	proc_sprintf(ofd, "in use:    %10zu (%10zx)\n", inuse, inuse);
	proc_sprintf(ofd, "leaked:    %10zu (%10zx)\n", leak, leak);
}

__MAYBE_UNUSED
static void pm_summary_imm() {
	/* last:
	 * 0: PM_NOMEM
	 * 1: PM_LEAK
	 * 2: PM_REF_ZERO
	 * 3: any references
	 */
	uint8_t last = 0;
	size_t base = 0, i = 0;
	size_t inuse = 0, avail = 0, leak = 0;

	for (; i < NBASE; i++) {
		int ref = base_page_refcounts[i];
		int dsp = disp(ref);

		if (dsp == 1)
			leak += PAGE_SIZE;
		if (dsp == 2)
			avail += PAGE_SIZE;
		if (dsp == 3)
			inuse += PAGE_SIZE;
		if (dsp == last)
			continue;

		if (i > 0)
			printf("%010zx %010zx %s\n", base, i * PAGE_SIZE, type(last));
		base = i * PAGE_SIZE;
		last = dsp;
	}

	printf("%010zx %010zx %s\n", base, i * PAGE_SIZE, type(last));

	printf("available: %10zu (%10zx)\n", avail, avail);
	printf("in use:    %10zu (%10zx)\n", inuse, inuse);
	printf("leaked:    %10zu (%10zx)\n", leak, leak);
}

int pm_avail() {
	int avail = 0;
	for (int i = 0; i < NBASE; i++) {
		if (base_page_refcounts[i] == PM_REF_ZERO)
			avail += PAGE_SIZE;
	}
	return avail;
}
