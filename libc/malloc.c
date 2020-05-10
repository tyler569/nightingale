
#include <basic.h>
#include <stdio.h>
#include <stdlib.h>
#undef free
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list.h>

#ifdef __kernel__
#include <ng/fs.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/syscall.h>
#include <ng/vmm.h>
#else // ! __kernel__
#include <sys/mman.h>
#endif // __kernel__

#include <assert.h>

#define ROUND_UP(x, to) ((x + to - 1) & ~(to - 1))
#define PTR_ADD(p, off) (void *)(((char *)p) + off)

#define DEBUGGING 0

#if __kernel__
#define debug_printf(...)
#define error_printf printf
#else
#define debug_printf(...)
#define error_printf printf
#endif

// possibility: each heap could use different magic numbers
#if __kernel__
#define MAGIC_NUMBER_1 0x4b4d454d // KMEM
#define MAGIC_NUMBER_2 0x4d454d4b // MEMK
#else
#define MAGIC_NUMBER_1 0x754d454d // uMEM
#define MAGIC_NUMBER_2 0x4d454d75 // MEMu
#endif

#define ALLOC_POISON 'M'
#define FREE_POISON 'F'

#ifndef __kernel__
#define mutex_await(...)
#define mutex_unlock(...)
#define KMUTEX_INIT_LIVE(...)
#endif // __kernel__


struct mheap _global_heap = {0};
struct mheap *global_heap = &_global_heap;

#if __kernel__
char early_malloc_pool[EARLY_MALLOC_POOL_LEN];
#endif

// Heap functions
#define HEAP_BASE_LEN (16 * 1024 * 1024)

void *heap_get_memory(size_t length) {
#ifdef __kernel__
        void *mem = (void *)vmm_reserve(length);
#else
        void *mem = mmap(NULL, length,
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS, -1, 0);
#endif
        return mem;
}

void _heap_expand(struct mheap *heap, void *region, size_t len) {
        struct free_mregion *new_region = (free_mregion *)region;

        new_region->m.magic_number_1 = MAGIC_NUMBER_1;
        new_region->m.length = len - sizeof(mregion);

        _list_append(&heap->free_list, &new_region->free_node);

        heap->total_size += len;
        heap->free_size += len;
}

void heap_expand(struct mheap *heap, size_t len) {
        void *region = heap_get_memory(len);
        _heap_expand(heap, region, len);
}

void heap_init(struct mheap *heap, void *region, size_t len) {
        list_init(&heap->free_list);
        heap->allocations = 0;
        heap->frees = 0;
        heap->total_size = 0;
        heap->free_size = 0;
        KMUTEX_INIT_LIVE(heap->lock);

        _heap_expand(heap, region, len);

        heap->is_init = true;
}

void nc_malloc_init(void) {
        size_t len = HEAP_BASE_LEN;
        void *region = heap_get_memory(len);
        heap_init(global_heap, region, len);
}

// Mregion functions

int mregion_validate(mregion *r) {
        return r->magic_number_1 == MAGIC_NUMBER_1; // &&
               //r->magic_number_2 == MAGIC_NUMBER_2;
}

struct mregion *mregion_of(void *ptr) {
        return PTR_ADD(ptr, -sizeof(mregion));
}

void *mregion_ptr(struct mregion *mr) {
        return PTR_ADD(mr, sizeof(mregion));
}

struct free_mregion *free_mregion_next(struct free_mregion *fmr) {
        return PTR_ADD(fmr, sizeof(mregion) + fmr->m.length);
}


struct free_mregion *mregion_split(struct free_mregion *fmr, size_t desired) {
        size_t real_split = round_up(desired, HEAP_MINIMUM_ALIGN);
        size_t len = fmr->m.length;

        size_t new_len = len - real_split - sizeof(mregion);
        if (new_len < HEAP_MINIMUM_BLOCK || new_len > 0xFFFFFFFF) {
                return NULL;
        }

        void *alloc_ptr = mregion_ptr((struct mregion *)fmr);
        struct free_mregion *new_region = PTR_ADD(alloc_ptr, real_split);
        new_region->m.magic_number_1 = MAGIC_NUMBER_1;
        // new_region->m.magic_number_2 = MAGIC_NUMBER_2;
        new_region->m.length = new_len;
        
        fmr->m.length = real_split;

        debug_printf("split -> %zu + %zu\n", fmr->m.length, new_region->m.length);

        return new_region;
}

struct free_mregion *mregion_merge(struct free_mregion *b, struct free_mregion *a) {
        if (free_mregion_next(b) != a) {
                return NULL;
        }

        b->m.length += sizeof(mregion);
        b->m.length += a->m.length;

        debug_printf("merge -> %zu\n", b->m.length);

        return b;
}
        

// Heap allocation functions

void *heap_malloc(struct mheap *heap, size_t len) {
        struct free_mregion *fmr;
        struct free_mregion *bestfit = NULL;
        bool found_any = false;
        assert(heap->is_init);
        mutex_await(&heap->lock);

        list_foreach(&heap->free_list, fmr, free_node) {
                if (fmr->m.length >= len) {
                        if (!found_any || fmr->m.length < bestfit->m.length) {
                                bestfit = fmr;
                        }
                        found_any = true;
                }
        }

        if (!found_any) {
                heap_expand(heap, round_up(len, 16 * 1024 * 1024));
                mutex_unlock(&heap->lock);
                return heap_malloc(heap, len);
        }


        struct free_mregion *after = mregion_split(bestfit, len);
        if (after) {
                _list_append(&bestfit->free_node, &after->free_node);
        }

        list_remove(&bestfit->free_node);
        struct mregion *mr = &bestfit->m;

        void *ptr = mregion_ptr((struct mregion *)bestfit);
        memset(ptr, ALLOC_POISON, mr->length);
        heap->allocations++;

        heap->free_size -= len;
        if (after) heap->free_size -= sizeof(mregion);

        if (heap->free_size < 64 * 1024) {
                heap_expand(heap, HEAP_BASE_LEN);
        }

        mutex_unlock(&heap->lock);
        return ptr;
}

void heap_free(struct mheap *heap, void *allocation) {
        mutex_await(&heap->lock);
        struct mregion *mr = mregion_of(allocation);
        if (!mregion_validate(mr)) {
                error_printf("invalid free of %p\n", allocation);
                return;
        }

        size_t allocation_len = mr->length;

        memset(allocation, FREE_POISON, mr->length);
        struct free_mregion *fmr = (struct free_mregion *)mr;
        struct free_mregion *fl;
        struct free_mregion *before = NULL;

        // Keep the free list sorted topologically
        list_foreach(&heap->free_list, fl, free_node) {
                if (fl > fmr) {
                        break;
                } else {
                        before = fl;
                }
        }

        heap->frees++;
        if (before && mregion_merge(before, fmr)) {
                heap->free_size += sizeof(mregion);
        } else if (before) {
                _list_append(&before->free_node, &fmr->free_node);
        } else {
                _list_prepend(&heap->free_list, &fmr->free_node);
        }

        heap->free_size += allocation_len;

        mutex_unlock(&heap->lock);
}

// realloc explicitly does not lock the heap FOR NOW since FOR NOW
// it only ever uses malloc and free, which each do.
void *heap_realloc(struct mheap *heap, void *allocation, size_t desired) {
        struct mregion *mr = mregion_of(allocation);
        if (!mregion_validate(mr)) {
                error_printf("invalid realloc of %p\n", allocation);
                return NULL;
        }

        void *new = heap_malloc(heap, desired);
        memcpy(new, allocation, mr->length);
        heap_free(heap, allocation);
        return new;
}

/*
int heap_contains(struct mheap *heap, void *allocation) {
        return (allocation >= PTR_ADD(heap->mregion_zero, sizeof(mregion)) &&
                PTR_ADD(heap->mregion_zero, heap->length) < allocation);
}
*/


// Global allocator functions

void *malloc(size_t len) {
        void *allocation = heap_malloc(global_heap, len);
        return allocation;
}

void free(void *allocation) {
        return heap_free(global_heap, allocation);
}

void *realloc(void *allocation, size_t desired) {
        void *out = heap_realloc(global_heap, allocation, desired);
        return out;
}

void *calloc(size_t count, size_t len) {
        void *allocation = heap_malloc(global_heap, count * len);
        memset(allocation, 0, count * len);
        return allocation;
}

void *zmalloc(size_t len) {
        return calloc(1, len);
}
