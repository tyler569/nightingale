
/*
 * kernel memory management
 *  for buffers, etc.
 * kmalloc, kcalloc, krealloc, kfree
 */

#include <stdint.h>
#include <stddef.h>
#include <kernel/kmem.h>

extern uintptr_t end_kernel;

uintptr_t malloc_base;
uintptr_t malloc_top;

void malloc_init() {
    malloc_base = (uintptr_t)(&end_kernel + 0xF) & ~0xF;
    malloc_top = (uintptr_t)(&end_kernel + 0x3FFFFF) & ~0x3FFFFF;
}

void *kmalloc(size_t size) {
    void *ret;
    int status;
    if (size < 16) {
        size = 16;
    }
    while (malloc_top - malloc_base < size) {
        status = alloc_4M_page(KERNEL_PD, malloc_top);
        
        if (status == OUT_OF_MEMORY) {
            return NULL;
        } else {
            malloc_top += 0x400000;
        }
    }
    
    ret = (void *)malloc_base;
    malloc_base += size;
    return ret;
}

void kfree(void *memory) {
    return;
}
