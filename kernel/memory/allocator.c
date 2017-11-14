
#include <stdbool.h>
#include <stddef.h>

#ifdef DEBUG
#include <debug.h>
#endif

#include "allocator.h"

#define BOOT_HEAP_SIZE 0x10000
extern void *initial_heap;

#define MINIMUM_BLOCK 32

void *current_position;

typedef struct MBlock {
    size_t len;
    bool is_free;
    struct MBlock *next;
} MBlock;

void heap_init() {
    MBlock *init = (MBlock *)initial_heap;
    init->len = BOOT_HEAP_SIZE;
    init->is_free = true;
    init->next = NULL;
}

void *malloc(size_t s) {

    /* round s to a multiple of 8 bytes. */
    s = s + 7 & ~7;

    DEBUG_PRINTF("malloc(%i)\n", s);

    MBlock *cur;
    MBlock *init = (MBlock *)initial_heap;
    for (cur = init; !cur->is_free && cur->len < s; cur = cur->next) {
        if (cur->next == NULL) {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

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
    }
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    MBlock *cur = (MBlock *)(v - sizeof(MBlock));
    cur->is_free = true;
}

