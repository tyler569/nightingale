
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * A testbed for a new implementation of malloc/realloc/free for nightingale.
 *
 * Goals:
 *  - support alligned alloc up to 1 page (4096B)
 *  - efficiency (memory and time)
 */

char* memory;

short* pmemory_map; // 16 bit refcount per page
char*  vmemory_map; // 1 bit per page

void* n_malloc(size_t len);
void* n_calloc(size_t len, size_t count);
void* n_realloc(void* mem, size_t len);
void n_free(void* mem);

struct mem { void* ptr; size_t len; }; // Andrei would say --
typedef struct mem mem;

enum owns { NO = 0, YES = 1, UNKNOWN = -1 };

#define X86_PAGE_LEN    0x1000
#define X86_PAGE_MASK    0xFFF
#define PAGE_LEN        X86_PAGE_LEN
#define PAGE_MASK       X86_PAGE_MASK

#define DEBUG 1
#define DEBUG_PRINTF(fmt, ...) \
    do { if (DEBUG) printf(" :" fmt, ## __VA_ARGS__); } while (0);

struct pfl_allocator {
    void* region;
    size_t region_pages;
    void* top_page;
    size_t page_stack_count;
    void* next_linear_page;
};

void pfl_init(struct pfl_allocator* pfl, void* region, size_t pages) {
    DEBUG_PRINTF("pfl_init(%p, %p, %lu)\n", pfl, region, pages);
    pfl->top_page = NULL;
    pfl->region = region;
    pfl->region_pages = pages;
    pfl->page_stack_count = 0;
    pfl->next_linear_page = region;
    // pmm_create_unbacked_range(region, pages * PAGE_LEN);
}

bool pfl_owns(struct pfl_allocator* pfl, void* mem) {
    DEBUG_PRINTF("pfl_owns(%p, %p)\n", pfl, mem);
    uintptr_t m = (uintptr_t)mem;
    if (m & PAGE_MASK) {
        return NO;
    }
    if (mem >= pfl->region && mem < pfl->region +
            pfl->region_pages * PAGE_LEN) {
        return YES;
    } else {
        return NO;
    }
}

void* pfl_allocate(struct pfl_allocator* pfl, size_t len) {
    if (len & PAGE_MASK) {
        return NULL; // round up?
    }
    if (pfl->top_page) {
        void* ret = pfl->top_page;
        if (pfl->page_stack_count > 1) {
            pfl->top_page = *(void**)pfl->top_page;
        } else {
            pfl->top_page = NULL;
        }
        pfl->page_stack_count -= 1;
        DEBUG_PRINTF("pfl_allocate(%p, %lu) -> %p\n", pfl, len, ret);
        return ret;
    } else if (pfl->next_linear_page) {
        void* ret = pfl->next_linear_page;
        pfl->next_linear_page += PAGE_LEN;
        if (pfl->next_linear_page >= pfl->region +
                pfl->region_pages * PAGE_LEN) {
            pfl->next_linear_page = NULL;
        }
        DEBUG_PRINTF("pfl_allocate(%p, %lu) -> %p\n", pfl, len, ret);
        return ret;
    } else {
        // oom, no pages
        DEBUG_PRINTF("pfl_allocate(%p, %lu) -> NULL\n", pfl, len);
        return NULL;
    }
}
void pfl_deallocate(struct pfl_allocator* pfl, void* ptr) {
    DEBUG_PRINTF("pfl_deallocate(%p, %p)\n", pfl, ptr);
    if (!pfl_owns(pfl, ptr)) {
        return;
    }
    void* old_top = pfl->top_page;
    *(void**)ptr = old_top;
    pfl->top_page = ptr;
    pfl->page_stack_count += 1;
}

struct bmb_allocator {
    size_t owned_len;
    void* region;
    size_t obj_size;
    char* bitmap; // 1 page == 4096 * 8 == 32k buckets;
};

void bmb_init(struct bmb_allocator* bmb, void* region, size_t len,
              size_t bucket, char* bitmap) {
    memset(bitmap, 0, len / bucket / 8);
    bmb->owned_len = len;
    bmb->region = region;
    bmb->obj_size = bucket;
    bmb->bitmap = bitmap;
}

bool bmb_owns(struct bmb_allocator* bmb, void* mem) {
    if (mem > bmb->region && mem <= bmb->region + bmb->owned_len) {
        return YES;
    } else {
        return NO;
    }
}

void* bmb_allocate(struct bmb_allocator* bmb, size_t len) {
    if (len > bmb->obj_size) {
        return NULL;
    }
    // scan bitmap - is there a better way?
    // *(uint64_t*)bitmap != 0xFFFFFFFFFFFFFFFF
}

void bmb_deallocate(struct bmb_allocator* bmb, void* ptr) {
    
}

struct allocator {
    void* (*allocate)(void* allocator, size_t);
    void (*deallocate)(void* allocator, void*);
    bool (*owns)(void* allocator, void*);
};

struct fallback_allocator {};

int main() {
    memory = malloc(128 * 1024 * 1024); // 128M == 32K pages

    void* alligned_mem = (void*)(((uintptr_t)memory+PAGE_MASK)&~PAGE_MASK);
    memory = alligned_mem;

    struct pfl_allocator pfl;
    pfl_init(&pfl, memory, 16);

    void* allocs[16];
    for (int i=0; i<16; i++) {
        allocs[i] = pfl_allocate(&pfl, PAGE_LEN);
    }
    for (int i=15; i>=0; i--) {
        pfl_deallocate(&pfl, allocs[i]);
    }
    for (int i=0; i<16; i++) {
        allocs[i] = pfl_allocate(&pfl, PAGE_LEN);
    }

}

