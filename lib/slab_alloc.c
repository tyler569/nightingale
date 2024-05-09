#include "assert.h"
#include "list.h"
#include "ng/arch-2.h"
#include "ng/mem.h"
#include "string.h"
#include "sys/slab.h"
#include "sys/spinlock.h"

LIST_HEAD(slab_caches);

void init_slab_cache(struct slab_cache *cache, size_t size) {
	assert(size < 16 * PAGE_SIZE && "just allocate pages");

	list_init(&cache->list);
	list_init(&cache->slabs_full);
	list_init(&cache->slabs_partial);
	list_init(&cache->slabs_free);

	list_append(&cache->list, &slab_caches);

	cache->object_size = size;

	// This formula gives us 254 objects per slab page for 16-byte objects,
	// reducing as the object size increases down to 32 objects per slab page
	// at a minimum for objects larger than 4096 bytes.
	cache->slab_page_count = 1 + (32 * size) / PAGE_SIZE;
}

#define BITMAP_SIZE (sizeof(uint64_t) * 4)

static size_t obj_per_page(struct slab_cache *cache) {
	return (PAGE_SIZE * cache->slab_page_count - BITMAP_SIZE)
		/ cache->object_size;
}

static void *obj_addr(page_t *page, size_t i, struct slab_cache *cache) {
	uintptr_t addr = page->slab_kernel_addr;
	assert(addr);

	return (void *)((uint8_t *)addr + BITMAP_SIZE + i * cache->object_size);
}

static bool obj_is_free(page_t *page, size_t i) {
	uintptr_t addr = page->slab_kernel_addr;
	assert(addr);

	return (((uint64_t *)addr)[i / 64] & (1ul << (i % 64))) == 0;
}

static void obj_set_bitmap(page_t *page, size_t i) {
	uintptr_t addr = page->slab_kernel_addr;
	assert(addr);

	((uint64_t *)addr)[i / 64] |= (1ul << (i % 64));
}

static void obj_clr_bitmap(page_t *page, size_t i) {
	uintptr_t addr = page->slab_kernel_addr;
	assert(addr);

	((uint64_t *)addr)[i / 64] &= ~(1ul << (i % 64));
}

static size_t page_free_index(page_t *page, size_t obj_per_page) {
	for (size_t i = 0; i < obj_per_page; i++) {
		if (obj_is_free(page, i))
			return i;
	}

	assert(0 && "no free index found");
}

static void up_ref_slab(struct slab_cache *cache, page_t *slab) {
	slab->slab_objects++;

	if (slab->slab_objects == obj_per_page(cache)) {
		list_remove(&slab->list);
		list_append(&slab->list, &cache->slabs_full);
	}
}

static void down_ref_slab(struct slab_cache *cache, page_t *slab) {
	if (slab->slab_objects == obj_per_page(cache)) {
		list_remove(&slab->list);
		list_append(&slab->list, &cache->slabs_partial);
	}

	slab->slab_objects--;

	if (slab->slab_objects == 0) {
		list_remove(&slab->list);
		list_append(&slab->list, &cache->slabs_free);
	}
}

static void grow_slab_cache(struct slab_cache *cache) {
	size_t slab_size = cache->slab_page_count * PAGE_SIZE;
	uintptr_t slab_vm_addr = alloc_kernel_vm(slab_size);
	uintptr_t vm_root = get_vm_root();

	page_t *slab_head = nullptr;

	for (size_t i = 0; i < cache->slab_page_count; i++) {
		page_t *page;
		uintptr_t addr = alloc_page_s(&page);

		assert(addr && "out of memory");

		add_vm_mapping(vm_root, slab_vm_addr + i * PAGE_SIZE, addr,
			PTE_PRESENT | PTE_WRITE);

		list_init(&page->list);

		if (i == 0) {
			slab_head = page;
			page->flags = PAGE_USED | PAGE_SLAB | PAGE_HEAD;
		} else
			page->flags = PAGE_USED | PAGE_SLAB | PAGE_MEMBER;

		page->slab_head = slab_head;
		page->slab_kernel_addr = slab_vm_addr;
		page->slab_cache = cache;
	}

	memset((void *)slab_vm_addr, 0, slab_size);

	list_append(&slab_head->list, &cache->slabs_free);
}

void *slab_alloc(struct slab_cache *cache) {
	assert(cache->object_size && "slab cache not initialized");

	spin_lock(&cache->lock);

	page_t *slab = nullptr;

	if (!list_empty(&cache->slabs_partial))
		slab = CONTAINER_OF(list_head(&cache->slabs_partial), page_t, list);

	if (!slab) {
		if (list_empty(&cache->slabs_free))
			grow_slab_cache(cache);

		slab = CONTAINER_OF(list_head(&cache->slabs_free), page_t, list);
		list_remove(&slab->list);
		list_append(&slab->list, &cache->slabs_partial);
	}

	size_t per_page = obj_per_page(cache);
	size_t index = page_free_index(slab, per_page);
	void *obj = obj_addr(slab, index, cache);
	obj_set_bitmap(slab, index);
	up_ref_slab(cache, slab);

	spin_unlock(&cache->lock);

	return obj;
}

void slab_free(struct slab_cache *cache, void *ptr) {
	spin_lock(&cache->lock);

	uintptr_t vm_root = get_vm_root();
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t phys_addr = resolve_vm_mapping(vm_root, addr);

	page_t *page = get_page_struct(phys_addr);
	page_t *slab = page->slab_head;

	uintptr_t slab_addr = slab->slab_kernel_addr;
	size_t index = (addr - slab_addr - BITMAP_SIZE) / cache->object_size;

	obj_clr_bitmap(slab, index);
	down_ref_slab(cache, slab);

	spin_unlock(&cache->lock);
}
