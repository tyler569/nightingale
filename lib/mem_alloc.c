#include "assert.h"
#include "ng/arch-2.h"
#include "ng/mem.h"
#include "sys/cdefs.h"
#include "sys/slab.h"

struct malloc_bucket {
	const size_t size;
	struct slab_cache cache;
};

static struct malloc_bucket buckets[] = { { .size = 16 }, { .size = 24 },
	{ .size = 32 }, { .size = 48 }, { .size = 64 }, { .size = 96 },
	{ .size = 128 }, { .size = 192 }, { .size = 256 }, { .size = 384 },
	{ .size = 512 }, { .size = 768 }, { .size = 1024 }, { .size = 1536 },
	{ .size = 2048 }, { .size = 3072 }, { .size = 4096 }, { .size = 6144 },
	{ .size = 8192 } };

static size_t sat_sub64(size_t a, size_t b) {
	size_t res = a - b;
	res &= -(res <= a);

	return res;
}

static size_t get_bucket_index_analytic(size_t size) {
	if (UNLIKELY(size < 2))
		return 0;

	size_t sm1 = size - 1;
	size_t l2 = 63 - __builtin_clzl(sm1);
	size_t bit2 = sm1 & (1ul << (l2 - 1));

	return sat_sub64(l2 * 2 + (bit2 == 0 ? 0 : 1), 7);
}

void init_kmem_alloc() {
	for (size_t i = 0; i < ARRAY_SIZE(buckets); i++) {
		struct slab_cache *cache = &buckets[i].cache;
		init_slab_cache(cache, buckets[i].size);
	}
}

void *kmem_page_alloc(size_t size) {
	size_t n_pages = ALIGN_UP(size, PAGE_SIZE) / PAGE_SIZE;
	uintptr_t vm_root = get_vm_root();
	uintptr_t vm_addr = alloc_kernel_vm(n_pages * PAGE_SIZE);
	page_t *alloc_head = nullptr;

	for (size_t i = 0; i < n_pages; i++) {
		page_t *page;
		uintptr_t addr = alloc_page_s(&page);
		add_vm_mapping(
			vm_root, vm_addr + i * PAGE_SIZE, addr, PTE_PRESENT | PTE_WRITE);

		if (i == 0) {
			alloc_head = page;
			page->flags = PAGE_USED | PAGE_ALLOC | PAGE_HEAD;
		} else
			page->flags = PAGE_USED | PAGE_ALLOC | PAGE_MEMBER;

		page->alloc_head = alloc_head;
		page->alloc_kernel_addr = vm_addr + i * PAGE_SIZE;
		page->alloc_pages = n_pages;
	}

	return (void *)vm_addr;
}

void kmem_page_free(page_t *page) {
	uintptr_t vm_root = get_vm_root();
	uintptr_t vm_addr = page->alloc_kernel_addr;
	size_t n_pages = page->alloc_pages;

	for (size_t i = 0; i < n_pages; i++) {
		uintptr_t addr = resolve_vm_mapping(vm_root, vm_addr + i * PAGE_SIZE);
		free_page(addr);
		add_vm_mapping(vm_root, vm_addr + i * PAGE_SIZE, 0, 0);
	}
}

void *kmem_alloc(size_t size) {
	if (size >= 8192)
		return kmem_page_alloc(size);

	size_t index = get_bucket_index_analytic(size);
	return slab_alloc(&buckets[index].cache);
}

void kmem_free(void *ptr) {
	uintptr_t vm_root = get_vm_root();
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t phys_addr = resolve_vm_mapping(vm_root, addr);

	page_t *page = get_page_struct(phys_addr);

	if (page->flags & PAGE_ALLOC)
		kmem_page_free(page);
	else if (page->flags & PAGE_SLAB)
		slab_free(page->slab_cache, ptr);
	else
		assert(0 && "invalid free");
}
