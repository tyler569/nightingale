
#pragma once
#ifndef NIGHTINGALE_MALLOC_H
#define NIGHTINGALE_MALLOC_H

#include <basic.h>

typedef struct mregion mregion;
struct mregion {
    unsigned long magic_number_1;
    mregion* previous;
    mregion* next;
    unsigned long length;
    int status;
    unsigned long magic_number_2;
};

void malloc_initialize(mregion* region_0, size_t pool_len);

void* pool_malloc(mregion* region_0, size_t len);
void* pool_calloc(mregion* region_0, size_t len, size_t count);
void* pool_realloc(mregion* region_0, void* allocation, size_t len);
void pool_free(mregion* region_0, void* allocation);

void print_pool(mregion* region_0);
void summarize_pool(mregion* region_0);

//
// the global "main" pool used by the kernel
// (should this be kmalloc?)
// this should be moved to stdlib.h, but as an interim, here we are.
//

#if X86_64
#define KMALLOC_GLOBAL_POOL (void*)0xFFFFFF0000000000
#elif I686
#define KMALLOC_GLOBAL_POOL (void*)0xC0000000
#endif
#define KMALLOC_GLOBAL_POOL_LEN (1<<24) // 16MB
extern mregion* kmalloc_global_region0;

void* malloc(size_t len);
void* calloc(size_t len, size_t count);
void* realloc(void* allocation, size_t len);
void free(void* allocation);

#endif
