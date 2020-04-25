
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
#endif // __kernel__


struct mheap global_heap = {0};

#if __kernel__
char early_malloc_heap[EARLY_MALLOC_HEAP_LEN];
struct mheap early_heap;
#endif

// Heap functions

void heap_init(struct mheap *heap, void *base, size_t len) {
        list_init(&heap->free_list);
        heap->length = len;
        heap->mregion_zero = base;
        heap->allocations = 0;
        heap->frees = 0;

        mregion *region_0 = heap->mregion_zero;

        region_0->magic_number_1 = MAGIC_NUMBER_1;
        region_0->length = len - sizeof(mregion);
        // region_0->magic_number_2 = MAGIC_NUMBER_2;

        free_mregion *fmr0 = (free_mregion *)heap->mregion_zero;

        _list_append(&heap->free_list, &fmr0->free_node);
}

int nc_malloc_init(void) {
        size_t heap_len = 16 * 1024 * 1024;

#ifdef __kernel__
        void *heap_base = vmm_reserve(heap_len);
        // TODO: vmm_earlyreserve?
#else
        void *heap_base = mmap(NULL, heap_len, PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS, -1, 0);
#endif

        heap_init(&global_heap, heap_base, heap_len);
        return 1;
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
                error_printf("malloc found no viable slot for alloc %zu\n", len);
                mutex_unlock(&heap->lock);
                return NULL;
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
                // nop
        } else if (before) {
                _list_append(&before->free_node, &fmr->free_node);
        } else {
                _list_prepend(&heap->free_list, &fmr->free_node);
        }
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

int heap_contains(struct mheap *heap, void *allocation) {
        return (allocation >= PTR_ADD(heap->mregion_zero, sizeof(mregion)) &&
                PTR_ADD(heap->mregion_zero, heap->length) < allocation);
}


// Global allocator functions

void *malloc(size_t len) {
        return heap_malloc(&global_heap, len);
}

void free(void *allocation) {
        return heap_free(&global_heap, allocation);
}

void *realloc(void *allocation, size_t desired) {
        return heap_realloc(&global_heap, allocation, desired);
}

void *calloc(size_t count, size_t len) {
        void *region = heap_malloc(&global_heap, count * len);
        memset(region, 0, count * len);
        return region;
}

void *zmalloc(size_t len) {
        return calloc(1, len);
}
