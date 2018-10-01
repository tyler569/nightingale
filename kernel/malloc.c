
#include <basic.h>

// #define DEBUG
#include <debug.h>
#include <panic.h>
#include <string.h>
#include <mutex.h>
#include "pmm.h"
#include "vmm.h"
#include "malloc.h"

#define MINIMUM_BLOCK 16

#if X86_64
# define MALLOC_STARTS_AT 0xFFFFFF0000000000
#elif I686
# define MALLOC_STARTS_AT 0xC0000000
#endif

char* malloc_current_top = (char*)MALLOC_STARTS_AT;

void* malloc(size_t n) {
    DEBUG_PRINTF("malloc(%zu)\n", n);

    char* allocation = malloc_current_top;

    vmm_create_unbacked_range(
            (uintptr_t)malloc_current_top, n + 32, PAGE_WRITEABLE);

    malloc_current_top += round_up(n, MINIMUM_BLOCK);
    for (int i=0; i<16; i+=2) {
        // malloc buffer zone 'XYXY'
        malloc_current_top[i]   = 'X';
        malloc_current_top[i+1] = 'Y';
    }
    malloc_current_top += 16;

    for (int i=0; i<n; i+=1) {
        // new allocation all 'M's
        allocation[i] = 'M';
    }

    return allocation;
}

void free(void* alloc) {
    DEBUG_PRINTF("free(%p)\n", alloc);

    char* allocation = alloc;
    for (int i=0; i<16; i++) {
        // every allocation takes at least 16 bytes
        // so this is good for now
        allocation[i] = 'F';
    }
    return;
}

void* realloc(void* allocation, size_t new_len) {
    DEBUG_PRINTF("realloc(%p, %zu)\n", allocation, new_len);

    void* new = malloc(new_len);
    memcpy(new, allocation, new_len);
    free(allocation);
    return new;
}

void* calloc(size_t n, size_t count) {
    DEBUG_PRINTF("calloc(%zu, %zu)\n", n, count);

    char* allocation = malloc(n * count);
    memset(allocation, 0, n);
    return allocation;
}

