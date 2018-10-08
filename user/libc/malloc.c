
#include <basic.h>
#include <string.h>
#include <stddef.h>
#include <sys/mman.h>
#include <stdlib.h>

#define MINIMUM_BLOCK 16

char* malloc_current_top = 0;

void malloc_init(void) {
    malloc_current_top = mmap(NULL, 0x100000, PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS, -1, 0);
}

void* malloc(size_t n) {
    char* allocation = malloc_current_top;

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
    char* allocation = alloc;
    for (int i=0; i<16; i++) {
        // every allocation takes at least 16 bytes
        // so this is good for now
        allocation[i] = 'F';
    }
    return;
}

void* realloc(void* allocation, size_t new_len) {
    void* new = malloc(new_len);
    memcpy(new, allocation, new_len);
    free(allocation);
    return new;
}

void* calloc(size_t n, size_t count) {
    char* allocation = malloc(n * count);
    memset(allocation, 0, n);
    return allocation;
}

