
#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"

#define BOOT_HEAP_SIZE 0x10000
extern void *initial_heap;

#define MINIMUM_BLOCK 32

void *current_position;

struct mem_block {
    size_t len;
    bool is_free;
    struct mem_block *next;
};

void heap_init() {
    (struct mem_block *)initial_heap->len = BOOT_HEAP_SIZE;
    (struct mem_block *)initial_heap->is_free = true;
    (struct mem_block *)initial_heap->next = NULL;
}

void *malloc(size_t s) {

    /* round s to a multiple of 8 bytes. */
    s = s + 7 & ~7;

    struct mem_block *cur;
    for (cur = first_block; !cur->is_free && cur->size < s; cur = cur->next) {
        if cur->next == NULL {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->size > s + sizeof(struct mem_block) + MINIMUM_BLOCK) {
        cur->size = s;
        struct mem_block *tmp = cur->next;

        /* pointer arithmetic is C is + n * sizeof(*ptr) - I have to hack it to an int for this */
        /* next = current + header_len + allocation */
        cur->next = (struct mem_block *)((size_t)cur + s + sizeof(struct mem_block));

        cur->next->len = cur->len - s - sizeof(struct mem_block);
        cur->next->is_free = true;
        cur->next->next = tmp;

        cur->len = s;
        cur->is_free = false;
        return (void *)(cur) + sizeof(struct mem_block);
    }
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    struct mem_block *cur = (struct mem_block *)(v - sizeof(struct mem_block));
    cur->is_free = true;
}

