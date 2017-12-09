
#include <stdbool.h>
#include <stddef.h>

#define DEBUG 0
#include <debug.h>

#include "allocator.h"

#define BOOT_HEAP_SIZE 0x10000
#define MINIMUM_BLOCK 32

void *current_position;

typedef struct MBlock {
    size_t len;
    bool is_free;
    struct MBlock *next;
} MBlock;

extern MBlock initial_heap;
MBlock *init = &initial_heap;

void heap_init() {
    init->len = BOOT_HEAP_SIZE;
    init->is_free = true;
    init->next = NULL;
}

void *malloc(size_t s) {

    /* round s to a multiple of 8 bytes. */
    s = s + 7 & ~7;

    DEBUG_PRINTF("malloc(%i)\n", s);

    MBlock *cur;
    for (cur = init; !(cur->is_free) || s > cur->len; cur = cur->next) {
        if (cur->next == NULL) {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

    //DEBUG_PRINTF("We can use %x!\n", cur);

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->len > s + sizeof(MBlock) + MINIMUM_BLOCK) {
        cur->len = s;
        MBlock *tmp = cur->next;

        /* pointer arithmetic is C is + n * sizeof(*ptr) - I have to hack it to an int for this */
        /* next = current + header_len + allocation */
        cur->next = (MBlock *)((size_t)cur + s + sizeof(MBlock));

        cur->next->len = cur->len - s - sizeof(MBlock);
        cur->next->is_free = true;
        cur->next->next = tmp;

        cur->len = s;
        cur->is_free = false;
        return (void *)(cur) + sizeof(MBlock);
    } else {
        cur->is_free = false;
        return (void *)(cur) + sizeof(MBlock);
    }

    DEBUG_PRINTF("error: malloc should never get here!\n");
    return NULL;
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    DEBUG_PRINTF("free(%x)\n", v);

    MBlock *cur = (MBlock *)(v - sizeof(MBlock));
    cur->is_free = true;
}

