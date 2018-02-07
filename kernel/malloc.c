
#include <basic.h>

//#define DEBUG
#include <debug.h>
#include <panic.h>

#include "pmm.h"
#include "vmm.h"
#include "malloc.h"

#define MINIMUM_BLOCK 32

/*
 * Come back later and optimize this.
 * Examples:
 *  - is_free could probably be a flag somewhere
 *  - have a previous block for combining?
 */
typedef struct Mem_Block {
    usize len;
    bool is_free;
    struct Mem_Block *next;
} Mem_Block;

// VOLATILE : this will eventually overwrite things
// I made an assert for this in main
static Mem_Block *init = (void *)0x1c0000;
static void *current_position; // why is this here?
static bool did_init = false;

void init_heap(usize start) {
    panic("Removed - delete references to this.\n");
}

static void back_memory(void *from, void *to) {
    printf("Backing %p to %p\n", from, to);

    if (to == NULL) {
        to = from;
    }
    usize first_page = (usize)from & ~PAGE_MASK_4K;
    usize last_page = (usize)to & ~PAGE_MASK_4K;

    for (usize page = first_page; page <= last_page; page += 0x1000) {
        if (page_resolve_vtop(page) == -1) {
            // in OOM, phy_allocate_page panics
            page_map_vtop(page, phy_allocate_page());
        }
        if (page_resolve_vtop(page) == -1) {
            panic("malloc: WTF, my memory isn't mapped!\n");
        }
    }
}

void *malloc(usize s) {
    if (!did_init) {
        init->len = 1L << 40; // close enough to infinity, and it even works with existing code 
        init->is_free = true;
        init->next = NULL;

        did_init = true;
    }

    /* round s to a multiple of 8 bytes. */
    s = s + 7 & ~7;

    DEBUG_PRINTF("malloc(%i)\n", s);

    Mem_Block *cur;
    for (cur = init; !(cur->is_free) || s > cur->len; cur = cur->next) {
        if (cur->next == NULL) {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

    //DEBUG_PRINTF("We can use %x!\n", cur);

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->len > s + sizeof(Mem_Block) + MINIMUM_BLOCK) {
        cur->len = s;
        Mem_Block *tmp = cur->next;

        /* pointer arithmetic is C is + n * sizeof(*ptr) - I have to hack it to an int for this */
        /* next = current + header_len + allocation */
        cur->next = (Mem_Block *)((usize)cur + s + sizeof(Mem_Block));
        back_memory(cur->next, cur->next + 1);

        cur->next->len = cur->len - s - sizeof(Mem_Block);
        cur->next->is_free = true;
        cur->next->next = tmp;

        cur->len = s;
        cur->is_free = false;

        back_memory(cur, cur->next);
        return (void *)(cur) + sizeof(Mem_Block);
    } else {
        cur->is_free = false;

        back_memory(cur, (void *)((usize)cur + cur->len));
        return (void *)(cur) + sizeof(Mem_Block);
    }

    WARN_PRINTF("error: malloc should never get here!\n");
    return NULL;
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    DEBUG_PRINTF("free(%x)\n", v);

    Mem_Block *cur = (Mem_Block *)(v - sizeof(Mem_Block));
    cur->is_free = true;
}

