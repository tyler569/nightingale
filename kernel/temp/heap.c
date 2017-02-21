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
#define HEAP_FULL_NODE_MAGIC  0xFACEFEED
#define HEAP_END 0xF0000000

/*
 * Empty space on the heap is identified with
 * this doubly linked list
 */
struct node {
    size_t len;
    struct node *next;
    struct node *prev;
    uint32_t magic;
} __attribute__((packed));

struct node *first_empty;

void heap_init() {
    vmm_alloc_page((uintptr_t)&kernel_end, PAGE_WRITE_ENABLE | PAGE_GLOBAL);

    first_empty = (struct node *)&kernel_end;
    first_empty->len = HEAP_END - (uintptr_t)&kernel_end;
    first_empty->next = NULL;
    first_empty->prev = NULL;
    first_empty->magic = HEAP_EMPTY_NODE_MAGIC;
}

void *heap_alloc(size_t size) {
    struct node *node = first_empty;
    while (node->len < size && node->magic != HEAP_EMPTY_NODE_MAGIC) {
        node = node->next;
        if (node == NULL) {
            klog("No space in heap for %lx", size);
            panic();
        }
    }
    // Now node contains a pointer to a sufficiently large
    // block of memory to fulfill the request.
    if (node->len - size > sizeof(struct node) + 4) {
        // Node can be broken up to support another allocation.
        size_t old_node_len = (size + sizeof(struct node) + 4) & ~3;
        size_t new_node_len = node->len - old_node_len;
        uintptr_t old_node_addr = (uintptr_t)node;
        uintptr_t new_node_addr = old_node_addr + old_node_len;
        struct node *new_node = (struct node *)new_node_addr;

        new_node->next = node->next;
        node->next = new_node;
        new_node->prev = node;
        node->len = old_node_len;
        new_node->len = new_node_len;
        node->magic = HEAP_FULL_NODE_MAGIC;
        new_node->magic = HEAP_EMPTY_NODE_MAGIC;

        return (void *)(node + 1);
    } else {
        node->magic = HEAP_FULL_NODE_MAGIC;
        return (void *)(node + 1);
    }
    return NULL;
}

void heap_free(void *memory) {
    return;
}

