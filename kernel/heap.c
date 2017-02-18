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
#include <kernel/mem/vmm.h>
#include <kernel/heap.h>

#include <kernel/printk.h> //debug

extern uintptr_t kernel_end;

#define HEAP_EMPTY_NODE_MAGIC 0xCAFEBABE
#define HEAP_END 0xF0000000

/*
 * Empty space on the heap is identified with
 * this doubly linked list
 */
struct empty_node {
    size_t block_len;
    struct empty_node *next;
    struct empty_node *prev;
    uint32_t magic;
};

struct empty_node *first_empty;

void heap_init() {
    vmm_alloc_page((uintptr_t)&kernel_end, PAGE_WRITE_ENABLE | PAGE_GLOBAL);

    first_empty = (struct empty_node *)&kernel_end;
    first_empty->block_len = HEAP_END - (uintptr_t)&kernel_end;
    first_empty->next = NULL;
    first_empty->prev = NULL;
    first_empty->magic = HEAP_EMPTY_NODE_MAGIC;
}

void *heap_alloc(size_t size) {
    return NULL;
}

void heap_free(void *memory) {
    return;
}

