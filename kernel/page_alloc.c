#include "assert.h"
#include "ng/mem.h"
#include "stdio.h"
#include "sys/spinlock.h"
#include <ng/arch-2.h>
#include <sys/cdefs.h>

static spin_lock_t page_lock;

static uintptr_t global_page_map_phy;

static page_t *global_page_map;
static size_t global_page_count;
static page_t *global_page_map_end;

// The global page map is a contiguous array of page_t structures that
// represent the physical memory of the system. Normally, global_page_map[0]
// represents the first page of physical memory at address 0, but on systems
// where this would create an unreasonable map (such as user-mode), the
// global_page_map_offset variable can be used to offset the map.
uintptr_t global_page_map_offset = 0;

LIST_HEAD(free_list);
static page_t *free_bump_cursor;

struct physical_extent extents[32];
size_t extent_count = ARRAY_SIZE(extents);

page_t *get_page_struct(uintptr_t addr) {
	size_t index = (addr - global_page_map_offset) / PAGE_SIZE;

	if (index >= global_page_count)
		return nullptr;

	return &global_page_map[index];
}

uintptr_t page_addr(page_t *page) {
	return (uintptr_t)(page - global_page_map) * PAGE_SIZE
		+ global_page_map_offset;
}

bool page_is_free(page_t *page) { return !(page->flags & PAGE_USED); }

// Print a number in SI units (e.g. 1.23 MB) out to 3 decimal places
void print_si_fraction(size_t num) {
	static const char *suffixes[] = { "kB", "MB", "GB", "TB" };

	if (num < 1024) {
		printf("%zu B", num);
		return;
	}

	for (int i = 0; i < 4; i++) {
		size_t p = 1024lu << (i * 10);
		if (num < p * 1024) {
			if (num % p == 0)
				printf("%zu %s", num / p, suffixes[i]);
			else
				printf(
					"%zu.%03zu %s", num / p, num % p * 1000 / p, suffixes[i]);
			break;
		}
	}
}

page_t *alloc_page_map(size_t num_pages) {
	for (size_t i = 0; i < extent_count; i++) {
		struct physical_extent *extent = &extents[i];

		if (extent->flags != PHYSICAL_EXTENT_FREE)
			continue;

		size_t extent_pages = extent->len / PAGE_SIZE;

		if (extent_pages < num_pages)
			continue;

		uintptr_t addr = extent->start;

		return (page_t *)direct_map_of(addr);
	}

	assert(0 && "unable to allocate page map");
}

void fill_page_map() {
	for (size_t i = 0; i < global_page_count; i++)
		global_page_map[i].flags = PAGE_UNUSABLE;

	for (size_t i = 0; i < extent_count; i++) {
		struct physical_extent *extent = &extents[i];

		for (size_t j = 0; j < extent->len / PAGE_SIZE; j++) {
			page_t *page = get_page_struct(extent->start + j * PAGE_SIZE);
			if (extent->flags & PHYSICAL_EXTENT_USED)
				page->flags = PAGE_USED;
			else
				page->flags = PAGE_FREE;
		}
	}

	for (uintptr_t addr = global_page_map_phy;
		 addr < global_page_map_phy + global_page_count * sizeof(page_t);
		 addr += PAGE_SIZE) {
		page_t *page = get_page_struct(addr);

		page->flags = PAGE_USED;
	}
}

void init_page_mmap() {
	get_physical_extents(extents, &extent_count);

	global_page_map_offset = extents[0].start;
	uintptr_t highest_usable_addr
		= extents[extent_count - 1].start + extents[extent_count - 1].len;

	size_t page_struct_size = sizeof(struct page);
	size_t memory_represented = highest_usable_addr - global_page_map_offset;
	size_t page_struct_count = memory_represented / PAGE_SIZE;
	size_t page_map_total_size
		= ALIGN_UP(page_struct_size * page_struct_count, PAGE_SIZE);

	printf("  Lowest usable address: %#zx\n", global_page_map_offset);
	printf("  Highest usable address: %#zx\n", highest_usable_addr);
	printf("  Page struct count: %zu\n", page_struct_count);
	printf("  Need %zu bytes for page structs\n", page_map_total_size);
	printf(
		"  Need %zu pages for page structs\n", page_map_total_size / PAGE_SIZE);

	printf("Page struct size: (");
	print_si_fraction(page_map_total_size);
	printf(")\n");

	global_page_map = alloc_page_map(page_map_total_size / PAGE_SIZE);
	global_page_map_phy = page_addr(global_page_map);
	global_page_count = page_struct_count;
	global_page_map_end = global_page_map + page_struct_count;
	free_bump_cursor = global_page_map;

	fill_page_map();
}

uintptr_t alloc_page_s(page_t **page_ret) {
	page_t *page = nullptr;

	spin_lock(&page_lock);

	if (!list_empty(&free_list)) {
		struct list_head *l = list_pop_front(&free_list);
		page = CONTAINER_OF(l, page_t, list);
	} else
		do
			if (page_is_free(free_bump_cursor)) {
				page = free_bump_cursor++;
				break;
			}
		while (++free_bump_cursor < global_page_map_end);

	page->flags = PAGE_USED;

	spin_unlock(&page_lock);

	if (page_ret)
		*page_ret = page;

	return page_addr(page);
}

void free_page(uintptr_t addr) {
	spin_lock(&page_lock);

	page_t *page = get_page_struct(addr);
	list_prepend(&page->list, &free_list);
	page->flags = PAGE_FREE;

	spin_unlock(&page_lock);
}
