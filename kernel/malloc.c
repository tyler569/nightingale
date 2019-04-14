
#include <basic.h>
#include <debug.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <panic.h>
#include <mutex.h>
#include <vmm.h>
#include "malloc.h"


#define PTR_ADD(p, off) (void*)(((char*)p) + off)

#define DEBUGGING 0
#define error_printf printf

#define MAGIC_NUMBER_1 (~12L)
#define MAGIC_NUMBER_2 (0x40048086L)

#define STATUS_FREE  (1)
#define STATUS_INUSE (2)

#define MINIMUM_BLOCK (1 << 8) // 256B - is this too low (or high)?
#define MINIMUM_ALIGN (1 << 4) // 16B

// don't split a region if we're going to use most of it anyway
// ( don't leave lots of tiny gaps )
int should_split_mregion(mregion* orig, size_t candidiate);

mregion* split_mregion(mregion* orig, size_t split_at, size_t align);
mregion* merge_mregion(mregion* r1, mregion* r2);

// check magics and status are valid
int validate_mregion(mregion* r);

void malloc_initialize(mregion* region_0, size_t len) {
    vmm_create_unbacked_range((uintptr_t)region_0, len, PAGE_WRITEABLE);

    region_0->magic_number_1 = MAGIC_NUMBER_1;
    region_0->previous = NULL;
    region_0->next = NULL;
    region_0->length = len - sizeof(mregion);
    region_0->status = STATUS_FREE;
    region_0->magic_number_2 = MAGIC_NUMBER_2;
}

int validate_mregion(mregion* r) {
    return
        r->magic_number_1 == MAGIC_NUMBER_1 &&
        r->magic_number_2 == MAGIC_NUMBER_2 &&
        (r->status & ~0x3) == 0;
}

int should_split_mregion(mregion* r, size_t candidate) {
    if (r->length < candidate) {
        // not big enough
        return 0;
    }

    if (r->length - candidate < MINIMUM_BLOCK + sizeof(mregion)) {
        // wouldn't leave enough space
        return 0;
    }
    // should we split in the case that we leave only a 256b hole?
    return 1;
}

mregion* split_mregion(mregion* r, size_t split_at, size_t align) {
    size_t actual_offset = round_up(split_at, MINIMUM_BLOCK);

    // calculate how much should be added to the offset so the final
    // pointer to the allocation is aligned
    uintptr_t new_alloc = (size_t)r + 2*sizeof(mregion) + actual_offset;
    uintptr_t aligned = round_up(new_alloc, align);
    actual_offset += aligned - new_alloc;

    if (!should_split_mregion(r, actual_offset)) {
        return NULL;
    }

    mregion* new_region = PTR_ADD(r, actual_offset + sizeof(mregion));
    new_region->magic_number_1 = MAGIC_NUMBER_1;
    new_region->previous = r;
    new_region->next = r->next;
    new_region->length = r->length - actual_offset - sizeof(mregion);
    new_region->status = STATUS_FREE;
    new_region->magic_number_2 = MAGIC_NUMBER_2;

    r->next = new_region;
    r->length = actual_offset;
    if (new_region->next) {
        new_region->next->previous = new_region;
    }

    return new_region;
}

mregion* merge_mregions(mregion* r1, mregion* r2) {
    if (r1->status & STATUS_INUSE || r2->status & STATUS_INUSE) {
        // can't update regions that are in use
        return NULL;
    }

    if (PTR_ADD(r1, sizeof(mregion) + r1->length) != r2) {
        error_printf("tried to merge discontinuous regions (loc compare)\n");
        return NULL;
    }

    if (r2->previous != r1 || r1->next != r2) {
        error_printf("tried to merge discontinuous regions (ptr compare)\n");
        return NULL;
    }

    r1->length += sizeof(mregion) + r2->length;
    r1->next = r2->next;
    if (r1->next) {
        r1->next->previous = r1;
    }
    
    return r1;
}

void* pool_aligned_alloc(mregion* region_0, size_t len, size_t align) {
    if (align < MINIMUM_ALIGN)  align = MINIMUM_ALIGN;

    mregion* cr;
    for (cr=region_0; cr; cr=cr->next) {
        if (cr->status == STATUS_FREE && cr->length >= len)
            break;
    }

    if (!cr) {
        error_printf("no region available to handle malloc(%lu)\n", len);
        return NULL;
    }

    split_mregion(cr, len, align);

    cr->status = STATUS_INUSE;
    return PTR_ADD(cr, sizeof(mregion));
}

void* pool_malloc(mregion* region_0, size_t len) {
    return pool_aligned_alloc(region_0, len, MINIMUM_ALIGN);
}

void* pool_realloc(mregion* region_0, void* allocation, size_t len) {
    if (!allocation) {
        return pool_malloc(region_0, len);
    }

    mregion* to_realloc = PTR_ADD(allocation, -sizeof(mregion));
    if (!validate_mregion(to_realloc)) {
        error_printf("invalid pointer passed to realloc: %p\n", allocation);
        return NULL;
    }

    if (len < to_realloc->length) {
        split_mregion(to_realloc, len, MINIMUM_ALIGN);
        return allocation;
    }

    size_t old_len = to_realloc->length;

    if (merge_mregions(to_realloc, to_realloc->next)) {
        if (len < to_realloc->length) {
            split_mregion(to_realloc, len, MINIMUM_ALIGN);
        }
        
        return allocation;
    }

    void* new_allocation = pool_malloc(region_0, len);
    if (!new_allocation)
        return NULL;
    memcpy(new_allocation, allocation, old_len);
    pool_free(region_0, allocation);
    return new_allocation;
}


void* pool_calloc(mregion* region_0, size_t count, size_t len) {
    void* alloc = pool_malloc(region_0, count * len);
    if (!alloc)
        return NULL;
    
    memset(alloc, 0, count * len);
    return alloc;
}

void pool_free(mregion* region_0, void* allocation) {
    if (!allocation) {
        // free(NULL) is a nop
        return;
    }

    mregion* to_free = PTR_ADD(allocation, -sizeof(mregion));
    if (!validate_mregion(to_free)) {
        error_printf("invalid pointer passed to free: %p\n", allocation);
        return;
    }
    to_free->status = STATUS_FREE;

    mregion* freed = to_free;
    if (to_free->previous) {
        freed = merge_mregions(to_free->previous, to_free);
    }
    if (freed && freed->next) {
        merge_mregions(freed, freed->next);
    } else if (!freed && to_free->next) {
        merge_mregions(to_free, to_free->next);
    }

    return;
}


// test stuff

void print_pool(mregion* region_0) {
    printf("region, next, previous, length, status, valid\n");

    mregion* r;
    for (r=region_0; r; r=r->next) {
        printf("%p, %p, %p, %lu, %s, %s\n",
                r, r->next, r->previous, r->length,
                  r->status == STATUS_FREE  ? "free" :
                  r->status == STATUS_INUSE ? "used" : " BAD",
                validate_mregion(r) ? "valid" : "INVALID");
    }
    printf("**\n\n");

    return;
}

void summarize_pool(mregion* region_0) {
    size_t inuse_len = 0;
    size_t total_len = 0;

    int inuse_regions = 0;
    int total_regions = 0;

    mregion* r;
    for (r=region_0; r; r=r->next) {
        if (r->status == STATUS_INUSE) {
            inuse_len += r->length;
            inuse_regions++;
        }
        total_len += r->length;
        total_regions++;

        if (!validate_mregion(r)) {
            printf("INVALID_REGION: %p\n", r);
        }
    }

    printf("total len: %zu\n", total_len);
    printf("inuse len: %zu\n", inuse_len);
    printf("total regions: %i\n", total_regions);
    printf("inuse regions: %i\n", inuse_regions);
}


// the global heap functions

mregion* kmalloc_global_region0 = KMALLOC_GLOBAL_POOL;
kmutex kmalloc_mutex = 0;

void* malloc(size_t len) {
    if (DEBUGGING)  printf("malloc(%zu) ", len);
    await_mutex(&kmalloc_mutex);
    void* alloc = pool_malloc(kmalloc_global_region0, len);
    release_mutex(&kmalloc_mutex);
    if (DEBUGGING)  printf("-> %p\n", alloc);
    return alloc;
}

void* calloc(size_t len, size_t count) {
    if (DEBUGGING)  printf("calloc(%zu, %zu)\n", len, count);
    await_mutex(&kmalloc_mutex);
    void* alloc = pool_calloc(kmalloc_global_region0, len, count);
    release_mutex(&kmalloc_mutex);
    return alloc;
}

void* realloc(void* allocation, size_t len) {
    if (DEBUGGING)  printf("realloc(%p, %zu)\n", allocation, len);
    await_mutex(&kmalloc_mutex);
    void* alloc = pool_realloc(kmalloc_global_region0, allocation, len);
    release_mutex(&kmalloc_mutex);
    return alloc;
}

void free(void* allocation) {
    if (DEBUGGING)  printf("free(%p)\n", allocation);
    await_mutex(&kmalloc_mutex);
    pool_free(kmalloc_global_region0, allocation);
    release_mutex(&kmalloc_mutex);
}

#include <syscall.h>

// Debug the kernel heap
struct syscall_ret sys_heapdbg(int type) {
    if (type == 1) {
        print_pool(kmalloc_global_region0);
    } else if (type == 2) {
        summarize_pool(kmalloc_global_region0);
    } else {
        RETURN_ERROR(EINVAL);
    }
    RETURN_VALUE(0);
}

