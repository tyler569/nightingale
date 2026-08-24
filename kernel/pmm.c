#include <assert.h>
#include <ng/arch.h>
#include <ng/early_alloc.h>
#include <ng/fs.h>
#include <ng/init.h>
#include <ng/mman.h>
#include <ng/pmm.h>
#include <ng/proc_files.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <stdio.h>

static spinlock_t pm_lock;

static constexpr size_t kb = 1024;
static constexpr size_t mb = kb * 1024;
static constexpr size_t gb = mb * 1024;
static constexpr size_t page_size = 4096;

static constexpr size_t number_of_regions = 128;
static size_t n_regions = number_of_regions;
static struct physical_region regions[number_of_regions];

static constexpr size_t max_early_address = 128 * mb;
static constexpr size_t n_early_pages = max_early_address / page_size;
__attribute__((aligned(4096))) struct page base_page_refcounts[n_early_pages];
static constexpr uintptr_t page_flat_map = 0xffff'8100'0000'0000;
struct page *pages = base_page_refcounts;

static size_t real_pages = n_early_pages;

static constexpr uint32_t page_exists_flag = 0x8000'0000;

static bool in_early_init() {
	return pages == base_page_refcounts;
}

static struct page *page_for_explicit(struct page *place, phys_addr_t page) {
	if (in_early_init() && page > max_early_address)
		return nullptr;
	return place + (page + 4095) / 4096;
}

static struct page *page_for(phys_addr_t page) {
	return page_for_explicit(pages, page);
}

static void set_explicit(struct page *place, phys_addr_t page, int refcount) {
	auto p = page_for_explicit(place, page);
	if (!p)
		return;
	p->refcount = page_exists_flag | refcount;
}

static void populate(struct page *place) {
	for (size_t i = 0; i < n_regions; i++) {
		auto region = &regions[i];
		if (region->type != prt_memory)
			continue;
		auto base = region->base;
		base += 4095;
		auto first_page = base / 4096;
		auto last_page = (region->base + region->len) / 4096;
		for (size_t n = first_page; n < last_page; n++)
			set_explicit(place, n * 4096, 0);
	}
}

static size_t max_page() {
	size_t max_page = 0;
	for (size_t i = 0; i < n_regions; i++) {
		auto region = &regions[i];
		auto top = region->base + region->len;
		if (top < max_page)
			continue;
		if (region->type == prt_memory)
			max_page = top;
	}
	// ceil divide
	max_page += 4095;
	return max_page / 4096;
}

void pmm_early_init() {
	arch_get_physical_regions(regions, &n_regions);

	populate(base_page_refcounts);
}
define_init(pmm_early_init, 0);

void pmm_init() {
	size_t max_pages = max_page();

	vmm_create_unbacked_range(
		page_flat_map, max_pages * sizeof(struct page), PAGE_WRITABLE);

	// add all the original mappings
	populate((struct page *)page_flat_map);

	// fix everything already allocated out of the bootstrap map
	memcpy((struct page *)page_flat_map, base_page_refcounts,
		sizeof(base_page_refcounts));

	pages = (struct page *)page_flat_map;
	real_pages = max_pages;

	// future work: free all of the memory associated with base_page_refcounts
	// for other uses
}
define_init(pmm_init, 2);

/* -============- */
// legacy interface

void pm_incref(phys_addr_t addr) {
	auto p = page_for(addr);
	if (!p)
		return;

	spin_lock(&pm_lock);

	assert(
		(p->refcount & page_exists_flag) != 0); // incref on page does not exist

	p->refcount += 1;

	spin_unlock(&pm_lock);
}

void pm_decref(phys_addr_t addr) {
	auto p = page_for(addr);
	if (!p)
		return;

	spin_lock(&pm_lock);

	assert(p->refcount & page_exists_flag); // decref on page does not exist
	assert(p->refcount > page_exists_flag); // decref on page with no references

	p->refcount -= 1;

	spin_unlock(&pm_lock);
}

phys_addr_t pm_alloc() {
	phys_addr_t addr = 0;

	spin_lock(&pm_lock);

	for (size_t i = 0; i < real_pages; i++) {
		auto p = &pages[i];
		if (p->refcount != 0x8000'0000)
			continue;
		p->refcount += 1;
		addr = i * 4096;
		break;
	}

	spin_unlock(&pm_lock);

	return addr;
}

void pm_free(phys_addr_t addr) {
	pm_decref(addr);
}

void proc_memdetail(struct file *ofd, void *) {
	for (size_t i = 0; i < real_pages; i++) {
		switch (pages[i].refcount) {
		case 0:
			proc_sprintf(ofd, "*");
			break;
		case 0x8000'0000:
			proc_sprintf(ofd, ".");
			break;
		default:
			proc_sprintf(ofd, "@");
			break;
		}

		if (i % 128 == 127)
			proc_sprintf(ofd, "\n");
	}
}
define_proc_file("memdetail", proc_memdetail, nullptr);

void proc_memcount(struct file *ofd, void *) {
	size_t total_mem = 0;
	size_t used_mem = 0;
	size_t reserved_mem = 0;

	for (size_t i = 0; i < real_pages; i++) {
		if (pages[i].refcount & page_exists_flag)
			total_mem++;
		else
			reserved_mem++;
		if (pages[i].refcount > page_exists_flag)
			used_mem++;
	}

	proc_sprintf(ofd, "total   :\t%zu\t%zu KiB\n", total_mem, total_mem * 4);
	proc_sprintf(ofd, "used    :\t%zu\t%zu KiB\n", used_mem, used_mem * 4);
	proc_sprintf(ofd, "free    :\t%zu\t%zu KiB\n", total_mem - used_mem,
		(total_mem - used_mem) * 4);
	proc_sprintf(
		ofd, "reserved:\t%zu\t%zu KiB\n", reserved_mem, reserved_mem * 4);
}
define_proc_file("memcount", proc_memcount, nullptr);
