#include <assert.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/mman.h>

#undef free

#define debug_printf(...)
#define error_printf printf

#define MAGIC_NUMBER_FREE 0x61626364 // 'abcd'
#define MAGIC_NUMBER_USED 0x41424344 // 'ABCD'

#define debug_printf(...)
#define error_printf printf
#define log_event(...)
#define spin_lock(...)
#define spin_unlock(...)

#define gassert assert
#define DEBUGGING 1

#define ALLOC_POISON 'M'
#define FREE_POISON 'F'

struct __ALIGN(16) mregion {
	unsigned int magic_number_1;
	size_t length;
};

struct free_mregion {
	struct mregion m;
	struct list_head free_node;
};

typedef struct mregion mregion;
typedef struct free_mregion free_mregion;

struct __mheap {
	struct list_head free_list;
	long allocations;
	long frees;
	size_t total_size;
	size_t free_size;
	bool is_init;
};

// for now I need the mregion to be N alignments wide exactly
static_assert(sizeof(struct mregion) % __HEAP_MINIMUM_ALIGN == 0);

// the free list node has to fit in a minimum-sized allocation block
static_assert(sizeof(free_mregion) - sizeof(mregion) <= __HEAP_MINIMUM_BLOCK);

struct __mheap _global_heap = { 0 };
struct __mheap *__global_heap_ptr = &_global_heap;

#if __kernel__
char early_malloc_pool[EARLY_MALLOC_POOL_LEN];
#endif

// Heap functions
#define HEAP_BASE_LEN (16 * 1024 * 1024)

static void *heap_get_memory(size_t length) {
#ifdef __kernel__
	uintptr_t vmm_reserve(size_t);

	void *mem = (void *)vmm_reserve(length);
#else
	void *mem = mmap(nullptr, length, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED) {
		// woops, and we can't printf yet either
		exit(123);
	}
#endif
	return mem;
}

static void _heap_expand(struct __mheap *heap, void *region, size_t len) {
	struct free_mregion *new_region = (free_mregion *)region;

	new_region->m.magic_number_1 = MAGIC_NUMBER_FREE;
	new_region->m.length = len - sizeof(mregion);

	list_append(&heap->free_list, &new_region->free_node);

	heap->total_size += len;
	heap->free_size += len;
}

static void heap_expand(struct __mheap *heap, size_t len) {
	void *region = heap_get_memory(len);
	_heap_expand(heap, region, len);
}

void heap_init(struct __mheap *heap, void *region, size_t len) {
	list_init(&heap->free_list);
	heap->allocations = 0;
	heap->frees = 0;
	heap->total_size = 0;
	heap->free_size = 0;

	_heap_expand(heap, region, len);

	heap->is_init = true;
}

void __nc_malloc_init() {
	size_t len = HEAP_BASE_LEN;
	void *region = heap_get_memory(len);
	heap_init(__global_heap_ptr, region, len);
}

// Mregion functions

static int mregion_validate(mregion *r, bool used) {
	/*
	if (used)
	return r->magic_number_1 == MAGIC_NUMBER_USED;
	else
	return r->magic_number_1 == MAGIC_NUMBER_FREE;
	 */
	return r->magic_number_1 == MAGIC_NUMBER_FREE
		|| r->magic_number_1 == MAGIC_NUMBER_USED;
}

static struct mregion *mregion_of(void *ptr) {
	return PTR_ADD(ptr, -sizeof(mregion));
}

static void *mregion_ptr(struct mregion *mr) {
	return PTR_ADD(mr, sizeof(mregion));
}

static struct free_mregion *free_mregion_next(struct free_mregion *fmr) {
	return PTR_ADD(fmr, sizeof(mregion) + fmr->m.length);
}

static void assert_consistency(struct list_head *free_list) {
	list_for_each_safe (free_list) {
		struct free_mregion *fmr
			= container_of(struct free_mregion, free_node, it);
		// assert(mregion_validate(&fmr->m, false));
		gassert(fmr->free_node.next->previous == &fmr->free_node);
	}
}

static struct free_mregion *mregion_split(
	struct __mheap *heap, struct free_mregion *fmr, size_t desired) {
	assert(mregion_validate(&fmr->m, false));
	size_t real_split = ROUND_UP(desired, __HEAP_MINIMUM_ALIGN);
	size_t len = fmr->m.length;

	size_t new_len = len - real_split - sizeof(mregion);
	if (new_len < __HEAP_MINIMUM_BLOCK || new_len > 0xFFFFFFFF)
		return nullptr;

	void *alloc_ptr = mregion_ptr((struct mregion *)fmr);
	struct free_mregion *new_region = PTR_ADD(alloc_ptr, real_split);
	new_region->m.magic_number_1 = MAGIC_NUMBER_FREE;
	new_region->m.length = new_len;

	fmr->m.length = real_split;

	debug_printf("split -> %zu + %zu\n", fmr->m.length, new_region->m.length);

	heap->free_size -= sizeof(struct mregion);

	return new_region;
}

static struct free_mregion *mregion_merge(
	struct __mheap *heap, struct free_mregion *b, struct free_mregion *a) {
	assert(mregion_validate(&a->m, false) && mregion_validate(&b->m, false));
	if (free_mregion_next(b) != a)
		return nullptr;

	b->m.length += sizeof(mregion);
	b->m.length += a->m.length;

	heap->free_size += sizeof(struct mregion);

	debug_printf("merge -> %zu\n", b->m.length);
	return b;
}

// Heap allocation functions

void *heap_malloc(struct __mheap *heap, size_t len) {
	struct free_mregion *bestfit = nullptr;
	bool found_any = false;
	assert(heap->is_init);

	if (len == 0)
		return nullptr;

	spin_lock(&heap->lock);

	if (DEBUGGING)
		assert_consistency(&heap->free_list);

	list_for_each_safe (&heap->free_list) {
		struct free_mregion *fmr
			= container_of(struct free_mregion, free_node, it);
		if (fmr->m.length >= len) {
			if (!found_any || fmr->m.length < bestfit->m.length) {
				bestfit = fmr;
			}
			found_any = true;
		}
	}

	if (!found_any) {
		heap_expand(heap, ROUND_UP(len + sizeof(mregion), 16 * 1024 * 1024));
		spin_unlock(&heap->lock);
		return heap_malloc(heap, len);
	}

	struct free_mregion *after = mregion_split(heap, bestfit, len);
	if (after)
		list_prepend(&bestfit->free_node, &after->free_node);

	list_remove(&bestfit->free_node);
	struct mregion *mr = &bestfit->m;

	void *ptr = mregion_ptr(mr);
	memset(ptr, ALLOC_POISON, mr->length);
	mr->magic_number_1 = MAGIC_NUMBER_USED;
	heap->allocations++;

	log_event(EVENT_ALLOC, "heap %p alloc of %zu (to %p)\n", heap, len, ptr);

	heap->free_size -= len;
	if (after)
		heap->free_size -= sizeof(mregion);

	if (heap->free_size < 64 * 1024)
		heap_expand(heap, HEAP_BASE_LEN);

	spin_unlock(&heap->lock);
	return ptr;
}

void heap_free(struct __mheap *heap, void *allocation) {
	if (!allocation)
		return;
	spin_lock(&heap->lock);
	struct mregion *mr = mregion_of(allocation);
	if (!mregion_validate(mr, true)) {
		error_printf("invalid free of %p\n", allocation);
		return;
	}

	if (DEBUGGING)
		assert_consistency(&heap->free_list);

	size_t allocation_len = mr->length;

	log_event(EVENT_FREE, "heap %p free of %p (%zu long)\n", heap, allocation,
		allocation_len);

	memset(allocation, FREE_POISON, mr->length);
	struct free_mregion *fmr = (struct free_mregion *)mr;
	struct free_mregion *before = nullptr;

	// Keep the free list sorted topologically
	list_for_each_safe (&heap->free_list) {
		struct free_mregion *fl
			= container_of(struct free_mregion, free_node, it);
		if (fl > fmr) {
			break;
		} else {
			before = fl;
		}
	}

	heap->frees++;
	if (before && mregion_merge(heap, before, fmr)) {
	} else if (before) {
		list_prepend(&before->free_node, &fmr->free_node);
	} else {
		list_prepend(&heap->free_list, &fmr->free_node);
	}

	heap->free_size += allocation_len;
	spin_unlock(&heap->lock);
}

// realloc explicitly does not lock the heap FOR NOW since FOR NOW
// it only ever uses malloc and free, which each do.
void *heap_realloc(struct __mheap *heap, void *allocation, size_t desired) {
	if (!allocation)
		return heap_malloc(heap, desired);

	struct mregion *mr = mregion_of(allocation);
	if (!mregion_validate(mr, true)) {
		error_printf("invalid realloc of %p\n", allocation);
		return nullptr;
	}

	size_t to_copy = MIN(mr->length, desired);

	void *new = heap_malloc(heap, desired);
	memcpy(new, allocation, to_copy);
	heap_free(heap, allocation);
	return new;
}

void *heap_zrealloc(struct __mheap *heap, void *allocation, size_t desired) {
	if (!allocation) {
		void *new = heap_malloc(heap, desired);
		memset(new, 0, desired);
		return new;
	}

	struct mregion *mr = mregion_of(allocation);
	if (!mregion_validate(mr, true)) {
		error_printf("invalid realloc of %p\n", allocation);
		return nullptr;
	}

	void *new = heap_malloc(heap, desired);
	memset(new, 0, desired);
	memcpy(new, allocation, mr->length);
	heap_free(heap, allocation);
	return new;
}

// Global allocator functions

void *malloc(size_t len) {
	void *allocation = heap_malloc(__global_heap_ptr, len);
	return allocation;
}

void free(void *allocation) { heap_free(__global_heap_ptr, allocation); }

void *realloc(void *allocation, size_t desired) {
	void *out = heap_realloc(__global_heap_ptr, allocation, desired);
	return out;
}

void *calloc(size_t count, size_t len) {
	void *allocation = heap_malloc(__global_heap_ptr, count * len);
	memset(allocation, 0, count * len);
	return allocation;
}

void *zmalloc(size_t len) { return calloc(1, len); }

void *zrealloc(void *allocation, size_t desired) {
	void *out = heap_zrealloc(__global_heap_ptr, allocation, desired);
	return out;
}
