/*
 * kernel memory management
 *  for buffers, etc.
 * kmalloc, kcalloc, krealloc, kfree
 */
 
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <multiboot.h>
#include <kernel/memory.h>

#include <kernel/printk.h> //debug

extern uintptr_t kernel_end;

#define EMPTY_NODE_MAGIC 0xCAFEBABE

/*
 * Empry space on the heap is identified with
 * this doubly linked list
 */
struct empty_node {
    size_t block_len;
    struct empty_node *next;
    struct empty_node *prev;
    uint32_t magic;
};

uintptr_t kmalloc_base;
uintptr_t kmalloc_top;

struct empty_node *first_empty;

void kmalloc_init() {
    kmalloc_base = (uintptr_t)&kernel_end;
    kmalloc_top = 0xFFC00000;

    first_empty = (struct empty_node *)kmalloc_base;
    first_empty->block_len = kmalloc_top - kmalloc_base;
    first_empty->next = NULL;
    first_empty->prev = NULL;
    first_empty->magic = EMPTY_NODE_MAGIC;
}

void *kmalloc(size_t size) {
    void *ret;
    int status;
    if (size < 16) {
        size = 16;
    }
    if (kmalloc_base + size < kmalloc_base) {
        return NULL;
    }
    while (kmalloc_top - kmalloc_base < size) {
        status = alloc_page(kmalloc_top);
        
        if (status == OUT_OF_MEMORY) {
            return NULL;
        } else {
            kmalloc_top += 0x400000;
        }
    }
    
    ret = (void *)kmalloc_base;
    kmalloc_base += size;
    return ret;
}

void kfree(void *memory) {
    return;
}

