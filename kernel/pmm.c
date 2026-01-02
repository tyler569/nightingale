#include <assert.h>
#include <limits.h>
#include <ng/fs.h>
#include <ng/pmm.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/x86/vmm.h>
#include <stdio.h>

static spinlock_t pm_lock;
static void *directory_pages_base;

static constexpr size_t kb = 1024;
static constexpr size_t mb = kb * 1024;
static constexpr size_t gb = mb * 1024;
static constexpr size_t page_size = 4096;
static constexpr size_t directory_span = 64 * mb;
static constexpr size_t max_memory = 32 * gb;

static constexpr size_t pages_per = directory_span / page_size;
static constexpr size_t max_pages = max_memory / page_size;
static constexpr size_t n_directories = max_pages / pages_per;
static constexpr size_t max_order = 14;
static constexpr uint32_t invalid_index = UINT32_MAX;

static_assert((pages_per & (pages_per - 1)) == 0, "pages_per must be power of two");
static_assert((1u << max_order) == pages_per, "max_order mismatch");

enum {
	PAGE_FLAG_FREE = 1u << 0,
};

struct pmm_directory {
	struct page *pages;
	uint32_t free_head[max_order + 1];
};

static struct page directory_0[pages_per];
static struct pmm_directory directories[n_directories];
static constexpr size_t directory_size = sizeof(struct page) * pages_per;
static size_t max_directory_in_use = 0;

static inline size_t pfn_from_pma(phys_addr_t pma) {
	return pma / page_size;
}

static inline size_t directory_index_for_pfn(size_t pfn) {
	return pfn / pages_per;
}

static inline uint32_t page_index_for_pfn(size_t pfn) {
	return (uint32_t)(pfn % pages_per);
}

static void directory_clear_free_lists(struct pmm_directory *dir) {
	for (size_t i = 0; i <= max_order; i++)
		dir->free_head[i] = invalid_index;
}

static void directory_init(struct pmm_directory *dir, struct page *pages) {
	dir->pages = pages;
	directory_clear_free_lists(dir);
	for (size_t i = 0; i < pages_per; i++) {
		struct page *page = &pages[i];
		page->refcount = PM_NOMEM;
		page->order = 0;
		page->flags = 0;
		page->next = invalid_index;
		page->prev = invalid_index;
		page->provenance = 0;
		page->aux = 0;
	}
}

void pm_init() {
	for (size_t i = 0; i < n_directories; i++) {
		directories[i].pages = nullptr;
		directory_clear_free_lists(&directories[i]);
	}
	directory_init(&directories[0], directory_0);
	max_directory_in_use = 0;

	// Reserve virtual space for all PMM directories upfront.
	// Each directory needs directory_size bytes, and we have n_directories of them.
	// This avoids the circular dependency of vmm_reserve -> page fault -> pm_alloc.
	size_t total_directory_space = directory_size * n_directories;
	directory_pages_base = vmm_hold(total_directory_space);
}

static struct pmm_directory *directory_get(size_t dir_index) {
	if (dir_index >= n_directories)
		return nullptr;
	if (!directories[dir_index].pages)
		return nullptr;
	return &directories[dir_index];
}

// Back a PMM directory by allocating physical pages and mapping them.
// This is called during pm_set when new physical memory regions are being added.
// It allocates from existing directories (like directory_0) to back new directories.
static struct page *pm_directory_back(size_t dir_index) {
	// Calculate the virtual address for this directory's page array.
	uintptr_t dir_vaddr = (uintptr_t)directory_pages_base + (dir_index * directory_size);

	// Allocate and map physical pages for this directory.
	// Each directory needs directory_size bytes, which is multiple pages.
	size_t n_pages = directory_size / page_size;
	for (size_t i = 0; i < n_pages; i++) {
		phys_addr_t pma = pm_alloc();
		virt_addr_t vma = dir_vaddr + (i * page_size);
		vmm_map(vma, pma, PAGE_PRESENT | PAGE_WRITEABLE);
	}

	return (struct page *)dir_vaddr;
}

static struct pmm_directory *directory_ensure(size_t dir_index) {
	if (dir_index >= n_directories)
		return nullptr;

	struct pmm_directory *dir = &directories[dir_index];
	if (dir->pages)
		return dir;

	struct page *pages = pm_directory_back(dir_index);
	directory_init(dir, pages);
	return dir;
}

static bool lookup_page(phys_addr_t pma, struct pmm_directory **out_dir,
	uint32_t *out_index, struct page **out_page) {
	size_t pfn = pfn_from_pma(pma);
	if (pfn >= max_pages)
		return false;

	size_t dir_index = directory_index_for_pfn(pfn);
	struct pmm_directory *dir = directory_get(dir_index);
	if (!dir)
		return false;

	uint32_t index = page_index_for_pfn(pfn);
	struct page *page = &dir->pages[index];

	if (out_dir)
		*out_dir = dir;
	if (out_index)
		*out_index = index;
	if (out_page)
		*out_page = page;
	return true;
}

static void buddy_remove(struct pmm_directory *dir, uint32_t index) {
	struct page *page = &dir->pages[index];
	uint16_t order = page->order;
	uint32_t next = page->next;
	uint32_t prev = page->prev;

	if (prev != invalid_index)
		dir->pages[prev].next = next;
	else
		dir->free_head[order] = next;
	if (next != invalid_index)
		dir->pages[next].prev = prev;

	page->next = invalid_index;
	page->prev = invalid_index;
	page->flags &= ~PAGE_FLAG_FREE;
}

static void buddy_add(struct pmm_directory *dir, uint32_t index, uint16_t order) {
	struct page *page = &dir->pages[index];
	page->order = order;
	page->flags |= PAGE_FLAG_FREE;
	page->prev = invalid_index;
	page->next = dir->free_head[order];
	if (page->next != invalid_index)
		dir->pages[page->next].prev = index;
	dir->free_head[order] = index;
}

static uint32_t buddy_alloc(struct pmm_directory *dir, uint16_t order) {
	for (uint16_t current = order; current <= max_order; current++) {
		uint32_t head = dir->free_head[current];
		if (head == invalid_index)
			continue;

		buddy_remove(dir, head);
		while (current > order) {
			current--;
			uint32_t buddy = head + (1u << current);
			buddy_add(dir, buddy, current);
		}
		return head;
	}

	return invalid_index;
}

static void buddy_free(struct pmm_directory *dir, uint32_t index, uint16_t order) {
	while (order < max_order) {
		uint32_t buddy = index ^ (1u << order);
		if (buddy >= pages_per)
			break;

		struct page *buddy_page = &dir->pages[buddy];
		if (!(buddy_page->flags & PAGE_FLAG_FREE) || buddy_page->order != order)
			break;

		buddy_remove(dir, buddy);
		if (buddy < index)
			index = buddy;
		order++;
	}

	buddy_add(dir, index, order);
}

static int pm_refcount_value(size_t pfn) {
	size_t dir_index = directory_index_for_pfn(pfn);
	struct pmm_directory *dir = directory_get(dir_index);
	if (!dir)
		return PM_NOMEM;

	return dir->pages[page_index_for_pfn(pfn)].refcount;
}

/*
 * -1: no such memory
 *  0: unused
 *  1+: used.
 */
int pm_refcount(phys_addr_t pma) {
	struct page *page = nullptr;
	if (!lookup_page(pma, nullptr, nullptr, &page))
		return -1;

	uint32_t value = page->refcount;
	if (value == PM_NOMEM) {
		return -1;
	} else if (value == PM_LEAK) {
		return 1;
	} else {
		return value - 2;
	}
}

int pm_incref(phys_addr_t pma) {
	struct pmm_directory *dir = nullptr;
	struct page *page = nullptr;
	uint32_t index = 0;
	if (!lookup_page(pma, &dir, &index, &page))
		return -1;

	uint32_t current = page->refcount;
	if (current < PM_REF_BASE) {
		// if the page is leaked or non-extant, just say it's still
		// in use and return.
		return 1;
	}

	spin_lock(&pm_lock);
	if (page->refcount == PM_REF_ZERO && (page->flags & PAGE_FLAG_FREE))
		buddy_remove(dir, index);
	page->refcount += 1;
	spin_unlock(&pm_lock);
	return page->refcount - PM_REF_ZERO;
}

int pm_decref(phys_addr_t pma) {
	struct pmm_directory *dir = nullptr;
	struct page *page = nullptr;
	uint32_t index = 0;
	if (!lookup_page(pma, &dir, &index, &page))
		return -1;

	uint32_t current = page->refcount;

	if (current < PM_REF_BASE) {
		// if the page is leaked or non-extant, just say it's still
		// in use and return.
		return 1;
	}

	// but _do_ error if the refcount is already zero, that means there's
	// a double free somewhere probably.
	assert(current != PM_REF_ZERO);

	spin_lock(&pm_lock);
	page->refcount -= 1;
	if (page->refcount == PM_REF_ZERO) {
		page->next = invalid_index;
		page->prev = invalid_index;
		page->flags &= ~PAGE_FLAG_FREE;
		buddy_free(dir, index, 0);
	}
	spin_unlock(&pm_lock);

	return page->refcount - PM_REF_ZERO;
}

void pm_set(phys_addr_t base, phys_addr_t top, uint32_t set_to) {
	phys_addr_t rbase = ROUND_DOWN(base, page_size);
	phys_addr_t rtop = ROUND_UP(top, page_size);

	for (phys_addr_t i = rbase; i < rtop; i += page_size) {
		size_t pfn = pfn_from_pma(i);
		if (pfn >= max_pages)
			break;

		size_t dir_index = directory_index_for_pfn(pfn);
		struct pmm_directory *dir = nullptr;
		if (set_to == PM_NOMEM) {
			dir = directory_get(dir_index);
			if (!dir)
				continue;
		} else {
			dir = directory_ensure(dir_index);
			if (!dir)
				continue;
			if (dir_index > max_directory_in_use)
				max_directory_in_use = dir_index;
		}

		uint32_t index = page_index_for_pfn(pfn);
		struct page *page = &dir->pages[index];

		spin_lock(&pm_lock);
		if (set_to == PM_REF_ZERO && page->refcount == PM_REF_ZERO
			&& (page->flags & PAGE_FLAG_FREE)) {
			spin_unlock(&pm_lock);
			continue;
		}
		page->refcount = set_to;
		page->next = invalid_index;
		page->prev = invalid_index;
		page->flags &= ~PAGE_FLAG_FREE;
		page->order = 0;
		if (set_to == PM_REF_ZERO)
			buddy_free(dir, index, 0);
		spin_unlock(&pm_lock);
	}
}

phys_addr_t pm_alloc() {
	spin_lock(&pm_lock);
	for (size_t d = 0; d < n_directories; d++) {
		struct pmm_directory *dir = &directories[d];
		if (!dir->pages)
			continue;

		uint32_t index = buddy_alloc(dir, 0);
		if (index == invalid_index)
			continue;

		struct page *page = &dir->pages[index];
		page->refcount = PM_REF_ZERO + 1;
		page->next = invalid_index;
		page->prev = invalid_index;
		page->flags &= ~PAGE_FLAG_FREE;
		page->order = 0;

		spin_unlock(&pm_lock);
		return (phys_addr_t)((d * pages_per + index) * page_size);
	}
	spin_unlock(&pm_lock);
	assert("no more physical pages" && 0);
	printf("WARNING: OOM\n");
	kill_process(running_process, 1);
	return 0;
}

phys_addr_t pm_alloc_contiguous(size_t n_pages) {
	if (n_pages == 0)
		return 0;
	if (n_pages == 1)
		return pm_alloc();
	if (n_pages > pages_per)
		goto oom;

	uint16_t order = 0;
	size_t block_pages = 1;
	while (block_pages < n_pages) {
		block_pages <<= 1;
		order++;
	}

	spin_lock(&pm_lock);
	for (size_t d = 0; d < n_directories; d++) {
		struct pmm_directory *dir = &directories[d];
		if (!dir->pages)
			continue;

		uint32_t index = buddy_alloc(dir, order);
		if (index == invalid_index)
			continue;

		for (size_t i = 0; i < n_pages; i++) {
			struct page *page = &dir->pages[index + i];
			page->refcount = PM_REF_ZERO + 1;
			page->next = invalid_index;
			page->prev = invalid_index;
			page->flags &= ~PAGE_FLAG_FREE;
			page->order = 0;
		}

		for (size_t i = n_pages; i < block_pages; i++) {
			struct page *page = &dir->pages[index + i];
			page->refcount = PM_REF_ZERO;
			page->next = invalid_index;
			page->prev = invalid_index;
			page->flags &= ~PAGE_FLAG_FREE;
			page->order = 0;
			buddy_free(dir, index + (uint32_t)i, 0);
		}

		spin_unlock(&pm_lock);
		return (phys_addr_t)((d * pages_per + index) * page_size);
	}
	spin_unlock(&pm_lock);

oom:
	printf("WARNING: OOM\n");
	kill_process(running_process, 1);
	return 0;
}

void pm_free(phys_addr_t pma) {
	pm_decref(pma);
}

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
	int last = 0;
	size_t base = 0, i = 0;
	size_t inuse = 0, avail = 0, leak = 0;

	size_t top_pages = (max_directory_in_use + 1) * pages_per;
	if (top_pages > max_pages)
		top_pages = max_pages;
	for (; i < top_pages; i++) {
		int ref = pm_refcount_value(i);
		int dsp = disp(ref);

		if (dsp == 1)
			leak += page_size;
		if (dsp == 2)
			avail += page_size;
		if (dsp == 3)
			inuse += page_size;
		if (dsp == last)
			continue;

		if (i > 0)
			proc_sprintf(
				ofd, "%010zx %010zx %s\n", base, i * page_size, type(last));
		base = i * page_size;
		last = dsp;
	}

	proc_sprintf(ofd, "%010zx %010zx %s\n", base, i * page_size, type(last));

	proc_sprintf(ofd, "available: %10zu (%10zx)\n", avail, avail);
	proc_sprintf(ofd, "in use:    %10zu (%10zx)\n", inuse, inuse);
	proc_sprintf(ofd, "leaked:    %10zu (%10zx)\n", leak, leak);
}

void pm_summary2(struct file *ofd, void *) {
	proc_sprintf(ofd, "directories:\n");
	for (size_t i = 0; i < n_directories; i++) {
		if (!directories[i].pages)
			continue;
		proc_sprintf(ofd, "  [%zu] = %p\n", i, directories[i].pages);
	}
}

size_t pm_avail() {
	size_t avail = 0;
	size_t top_pages = (max_directory_in_use + 1) * pages_per;
	if (top_pages > max_pages)
		top_pages = max_pages;
	for (size_t i = 0; i < top_pages; i++) {
		if (pm_refcount_value(i) == PM_REF_ZERO)
			avail += page_size;
	}
	return avail;
}
