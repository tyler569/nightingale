#pragma once

#include "list.h"
#include "stddef.h"
#include "stdint.h"

enum physical_extent_flags {
	PHYSICAL_EXTENT_FREE = 0,
	PHYSICAL_EXTENT_USED = 1 << 0,
};

struct physical_extent {
	enum physical_extent_flags flags;
	uintptr_t start;
	size_t len;
};

struct page {
	uint64_t flags;
	struct list_head list;
	union {
		struct {
			struct page *slab_head;
			struct slab_cache *slab_cache;
			uintptr_t slab_kernel_addr;
			uint32_t slab_objects;
		};
		struct {
			struct page *alloc_head;
			uintptr_t alloc_kernel_addr;
			uint32_t alloc_pages;
		};
	};
};

typedef struct page page_t;

enum page_flags : uint64_t {
	PAGE_UNUSABLE = 1 << 63,
	PAGE_FREE = 0,
	PAGE_USED = 1 << 0,
	PAGE_SLAB = 1 << 1,
	PAGE_ALLOC = 1 << 2,
	PAGE_HEAD = 1 << 3,
	PAGE_MEMBER = 1 << 4,
};

void print_si_fraction(size_t num);

void init_page_mmap();

uintptr_t alloc_page_s(page_t **page);
#define alloc_page() alloc_page_s(nullptr)
void free_page(uintptr_t);
page_t *get_page_struct(uintptr_t);

#define PTE_PRESENT (1 << 0)
#define PTE_WRITE (1 << 1)
#define PTE_USER (1 << 2)

uintptr_t alloc_kernel_vm(size_t len);
void add_vm_mapping(uintptr_t root, uintptr_t virt, uintptr_t phys, int flags);
uintptr_t resolve_vm_mapping(uintptr_t root, uintptr_t virt);
uintptr_t new_page_table();

void init_kmem_alloc();
void *kmem_alloc(size_t size);
void kmem_free(void *ptr);

// arch-specific

void get_physical_extents(struct physical_extent *extents, size_t *max);
uintptr_t direct_map_of(uintptr_t addr);
uintptr_t physical_of(uintptr_t addr);
