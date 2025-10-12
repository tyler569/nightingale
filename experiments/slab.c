#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "slab.h"

constexpr size_t page_size = 0x1000;
constexpr size_t waste_percentage = 10;

void *alloc_pages(size_t count) { return malloc(page_size * count); }

struct slab_header {
	struct slab_cache *cache;

	struct list_node siblings;

	void *base;
	size_t size;

	void *first_free;

	size_t object_count;
	size_t object_slots;
};

// objects absolutely must be stored as 8 bytes or larger for the free list
// anything larger than 8 bytes will store as a multiple of 16 for alignment
static size_t stored_size(size_t object_size) {
	if (object_size <= 8)
		return 8;
	else
		return (object_size + 15) & ~15;
}

// figure out how many pages we need per slab in order to not waste more than
// `waste_percentage` % of the allocated pages
static size_t pages_for_size(size_t object_size) {
	for (size_t pages = 1;; pages++) {
		size_t region_size = pages * page_size;
		size_t available_size = region_size - sizeof(struct slab_header);
		if ((available_size % object_size) * 100 / region_size
			< waste_percentage)
			return pages;
	}
}

static size_t objects_per_slab(size_t object_size, size_t pages) {
	return (pages * page_size - sizeof(struct slab_header)) / object_size;
}

static void *slab_object_ptr(struct slab_header *slab, size_t n) {
	size_t object_size = slab->cache->object_size;
	size_t object_count = slab->object_slots;

	if (n >= object_count)
		return nullptr;

	return slab->base + object_size * n;
}

static bool slab_contains_ptr(struct slab_header *slab, void *ptr) {
	return ptr >= slab->base && ptr < (void *)slab;
}

static struct slab_header *alloc_slab(struct slab_cache *cache) {
	size_t pages = pages_for_size(cache->object_size);
	size_t objects = objects_per_slab(cache->object_size, pages);

	void *slab = alloc_pages(pages);
	if (!slab)
		return nullptr;

	// header is placed at the _end_ of the allocated region
	struct slab_header *header
		= slab + (pages * page_size) - sizeof(struct slab_header);
	memset(header, 0, sizeof(struct slab_header));

	header->base = slab;
	header->size = pages * page_size;
	header->object_slots = objects;
	header->cache = cache;
	list_push_back(&cache->slabs, &header->siblings);

	// set up object free list
	for (size_t o = 0; o < objects - 1; o++) {
		*(void **)slab_object_ptr(header, o) = slab_object_ptr(header, o + 1);
	}
	*(void **)slab_object_ptr(header, objects - 1) = nullptr;

	header->first_free = slab;

	return header;
}

static void *alloc_object(struct slab_header *slab) {
	if (slab->object_count == slab->object_slots) {
		return nullptr;
	}

	void *object = slab->first_free;
	slab->first_free = *(void **)slab->first_free;
	slab->object_count += 1;
	return object;
}

static void free_object(struct slab_header *slab, void *ptr) {
	assert(slab_contains_ptr(slab, ptr));

	*(void **)ptr = slab->first_free;
	slab->first_free = ptr;
	slab->object_count -= 1;
}

void *slab_alloc(struct slab_cache *cache) {
	void *object = nullptr;

	list_for_each (&cache->slabs, s) {
		struct slab_header *slab = list_entry(s, struct slab_header, siblings);
		if ((object = alloc_object(slab)))
			return object;
	}

	struct slab_header *new_slab = alloc_slab(cache);
	if (new_slab)
		return alloc_object(new_slab);
	else
		return nullptr;
}

void slab_free(struct slab_cache *cache, void *ptr) {
	list_for_each (&cache->slabs, s) {
		struct slab_header *slab = list_entry(s, struct slab_header, siblings);
		if (slab_contains_ptr(slab, ptr)) {
			free_object(slab, ptr);
			return;
		}
	}

	assert(0);
}

void slab_cache_init(struct slab_cache *cache, size_t requested_size) {
	cache->requested_size = requested_size;
	cache->object_size = stored_size(requested_size);
	list_init(&cache->slabs);
}

void validate_page_sizes() {
	printf("size\tpages\tobjects\n");
	for (size_t i = 3; i < 15; i++) {
		size_t o = (1 << i);
		size_t p = pages_for_size(o);
		size_t n = objects_per_slab(o, p);
		printf("%zu\t%zu\t%zu\n", o, p, n);

		o = (1 << i) + (1 << (i - 1));
		p = pages_for_size(o);
		n = objects_per_slab(o, p);
		printf("%zu\t%zu\t%zu\n", o, p, n);
	}
}
