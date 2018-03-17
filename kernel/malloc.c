
#include <basic.h>

//#define DEBUG
#include <debug.h>
#include <panic.h>
#include <string.h>
#include <mutex.h>
#include "pmm.h"
#include "vmm.h"
#include "malloc.h"

#define MINIMUM_BLOCK 16

/*
 * Debugging tools for heap:
 *
 * strong_heap_protection defines some extra magic numbers
 * and tries to detect heap overruns (by panicing on the location
 * you'll then need to watchpoint that to figure out what
 * fucked you)
 *
 * never_release tells the heap to never actually mark memory free
 * though it will still overwrite it if you're running strong heap
 * protection
 */

#define __strong_heap_protection
#define __never_release

#define FREE_MAGIC  0xffffffff
#define INUSE_MAGIC 0xe5e5e5e5

struct block {
#ifdef __strong_heap_protection
    uint32_t magic;
#endif
    bool is_free;
    size_t len;
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

static kmutex malloc_lock = KMUTEX_INIT;

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

    await_mutex(&malloc_lock);

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
        if (cur->is_free == true) {
            if (cur->magic != FREE_MAGIC) {
                printf("magic: %#x\n", cur->magic);
                panic("heap corruption 1 detected: bad magic number at %#x\n", cur);
            }
        } else if (cur->is_free == false) {
            if (cur->magic != INUSE_MAGIC) {
                panic("heap corruption 2 detected: bad magic number at %#x\n", cur);
            }
            continue; // block is in use, continue
        } else {
            panic("heap corruption 5 detected: is_free not true or false at %#x\n", cur);
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

            release_mutex(&malloc_lock);
            return NULL;
        }
    }
    /* cur is now a block we can use */

    //DEBUG_PRINTF("We can use %x!\n", cur);

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->len > s + sizeof(struct block) + MINIMUM_BLOCK) {
        cur->len = s;
        struct block *tmp = cur->next;

        // pointer arithmetic is C is + n * sizeof(*ptr)
        // Gotta hack to an int for this
        // next = current + header_len + allocation
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

        release_mutex(&malloc_lock);
        return (void *)(cur) + sizeof(struct block);
    } else {
        cur->is_free = false;

#ifdef __strong_heap_protection
        cur->magic = INUSE_MAGIC;
#endif

        back_memory(cur, (void *)((usize)cur + cur->len));

        release_mutex(&malloc_lock);
        return (void *)(cur) + sizeof(struct block);
    }

    WARN_PRINTF("error: malloc should never get here!\n");

    release_mutex(&malloc_lock);
    return NULL;
}


/*
 * This needs to exist because realloc needs to be able to free
 * and can't await the already-taken mutex inside itself
 */

static void internal_nolock_free(void *v) {
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

#ifdef __never_release
    cur->is_free = true;
#endif

    if (cur->prev && cur->prev->is_free) {
        // combine cur and cur->prev
        // nop for now
    }

    if (cur->next && cur->next->is_free) {
        // combine cur and cur->next
        // nop for now
    }

#ifdef __strong_heap_protection

#ifdef __never_realse
    cur->magic = FREE_MAGIC;
#endif

    // always memset if strong_heap_protection
    memset(cur + 1, 0xab, cur->len);
#endif // __strong_heap_protection
}

void free(void *v) {
    await_mutex(&malloc_lock);

    internal_nolock_free(v);

    release_mutex(&malloc_lock);
}

void *realloc(void *v, size_t new_size) {
    if (new_size == 0) {
        internal_nolock_free(v);
    }

    struct block *cur = (struct block *)(v - sizeof(struct block));

    if (new_size <= cur->len) {
        // Do nothing for now, btu there is memory to be reclaimed

        release_mutex(&malloc_lock);
        return v;
    } else {
        // TODO: Check to see if the next block is free, and we can expand
        // without moving the memory!!!!!!!!!!!
        void *new = malloc(new_size);
        memcpy(v, new, cur->len); // does len include the header?  This could be too much
        internal_nolock_free(v);

        release_mutex(&malloc_lock);
        return new;
    }
}

