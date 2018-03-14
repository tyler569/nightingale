
#include <basic.h>

//#define DEBUG
#include <debug.h>
#include <panic.h>
#include <string.h>
#include "pmm.h"
#include "vmm.h"
#include "malloc.h"

#define MINIMUM_BLOCK 32

#define __strong_heap_protection

#define FREE_MAGIC 0x9293aafc
#define INUSE_MAGIC 0x19ba7d9d

struct block {
#ifdef __strong_heap_protection
    uint32_t magic;
#endif
    size_t len;
    bool is_free;
    struct block *next;
    struct block *prev;
};

// VOLATILE : this will eventually overwrite things
//
// Thought: this is just a virtual address, so maybe it can stay hardcoded
// forever.  It's not like starting the heap at 0xAnything would make a
// difference anyway, since it's not wasting real memory and is per-process
// anyway...
static struct block *init = (void *)0x1c0000;

// TODO: this or something else slightly smarter than what I have
// Use this variable or things like it to bring some more intelligence into
// this process.  I should either remember existing blocks of a few sizes
// or have pools or something.  As of now, each malloc is O(n) in the number
// of allocations already done.  Remember how slow it was to fill all memory?
// static void *current_position;

static bool did_init = false;

static void back_memory(void *from, void *to) {
    DEBUG_PRINTF("Backing %p to %p\n", from, to);

    if (to == NULL) {
        to = from;
    }
    usize first_page = (usize)from & PAGE_MASK_4K;
    usize last_page = (usize)to & PAGE_MASK_4K;

    for (usize page = first_page; page <= last_page; page += 0x1000) {
        if (vmm_virt_to_phy(page) == -1) {
            // in OOM, pmm_allocate_page panics
            vmm_map(page, pmm_allocate_page());
        }
        if (vmm_virt_to_phy(page) == -1) {
            panic("malloc: WTF, my memory isn't mapped!\n");
        }
    }
}

void *malloc(usize s) {

    // Instead of having a specific function to do something like malloc_init()
    // and just putting these values at the start of the heap, I just remember
    // whether we've already malloc'ed anything.  We already know the start of
    // the heap anyway (well, for now it's hardcoded), so it doesn't matter.
    if (!did_init) {
        // close enough to infinity, and it even works with existing code 
        // I need this because I don't want to and can not reliably limit
        // the size of the heap.  We just need to detect OOM when trying
        // to map a physical page, as we already do.
        init->len = 1L << 40;
        init->is_free = true;
#ifdef __strong_heap_protection
        init->magic = FREE_MAGIC;
#endif
        init->next = NULL;
        init->prev = NULL;

        did_init = true;
    }

    /* round s to a multiple of 8 bytes. */
    s = (s + 7) & ~7;

    DEBUG_PRINTF("malloc(%i)\n", s);

    struct block *cur;
    for (cur = init; ; cur = cur->next) {

#ifdef __strong_heap_protection
        if (cur->is_free) {
            if (cur->magic != FREE_MAGIC) {
                printf("magic: %#x\n", cur->magic);
                panic("heap corruption 1 detected: bad magic number at %#x\n", cur);
            }
        } else {
            if (cur->magic != INUSE_MAGIC) {
                panic("heap corruption 2 detected: bad magic number at %#x\n", cur);
            }
            continue; // block is in use, continue
        }
        if (cur->next) {
            if (cur->next->prev != cur) {
                panic("heap corruption 4 detected: bad n->p at %#x\n", cur);
            }
        }
#else
        if (!cur->is_free) {
            continue; // block is in use, continue
        }
#endif

        if (cur->len >= s) {
            break;
        }

        if (cur->next == NULL) {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

    //DEBUG_PRINTF("We can use %x!\n", cur);

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->len > s + sizeof(struct block) + MINIMUM_BLOCK) {
        cur->len = s;
        struct block *tmp = cur->next;

        /* pointer arithmetic is C is + n * sizeof(*ptr) - I have to hack it to an int for this */
        /* next = current + header_len + allocation */
        cur->next = (struct block *)((usize)cur + s + sizeof(struct block));
        back_memory(cur->next, cur->next + 1);

        cur->next->len = cur->len - s - sizeof(struct block);
        cur->next->is_free = true;

#ifdef __strong_heap_protection
        cur->next->magic = FREE_MAGIC;
#endif

        cur->next->next = tmp;
        cur->next->prev = cur;

        cur->len = s;
        cur->is_free = false;

#ifdef __strong_heap_protection
        cur->magic = INUSE_MAGIC;
#endif

        back_memory(cur, cur->next);
        return (void *)(cur) + sizeof(struct block);
    } else {
        cur->is_free = false;

#ifdef __strong_heap_protection
        cur->magic = INUSE_MAGIC;
#endif

        back_memory(cur, (void *)((usize)cur + cur->len));
        return (void *)(cur) + sizeof(struct block);
    }

    WARN_PRINTF("error: malloc should never get here!\n");
    return NULL;
}

void *realloc(void *v, size_t new_size) {
    if (new_size == 0) {
        free(v);
    }

    struct block *cur = (struct block *)(v - sizeof(struct block));

    if (new_size <= cur->len) {
        // Do nothing for now, btu there is memory to be reclaimed
        return v;
    } else {
        // TODO: Check to see if the next block is free, and we can expand
        // without moving the memory!!!!!!!!!!!
        void *new = malloc(new_size);
        memcpy(v, new, cur->len); // does len include the header?  This could be too much
        free(v);
        return new;
    }
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    DEBUG_PRINTF("free(%x)\n", v);

    struct block *cur = (struct block *)(v - sizeof(struct block));

#ifdef __strong_heap_protection
    if (cur->is_free) {
        panic("possible heap corruption - attempted double free of %#x\n", cur);
    }
    if (cur->magic != INUSE_MAGIC) {
        panic_bt("heap corruption 3 detected: bad magic number at %#x\n", cur);
    }
#endif

    cur->is_free = true;

    if (cur->prev && cur->prev->is_free) {
        // combine cur and cur->prev
    }

    if (cur->next && cur->next->is_free) {
        // combine cur and cur->next
    }

#ifdef __strong_heap_protection
    cur->magic = FREE_MAGIC;
    memset(cur + 1, 0xab, cur->len);
#endif
}

