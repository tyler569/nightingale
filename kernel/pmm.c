
#include <basic.h>

#define DEBUG
#include <debug.h>
#include <panic.h>
#include <mutex.h>
#include "pmm.h"

uintptr_t pmm_first_free_page;
uintptr_t pmm_last_page;
uintptr_t pmm_free_stack_size = 0;

/*
 *
 * TODO
 *
 * free stack
 *
 */

static kmutex pmm_lock = KMUTEX_INIT;

void pmm_allocator_init(uintptr_t first, uintptr_t last) {
    pmm_first_free_page = first;
    pmm_last_page = last;

    // Set up free stack
    
}

uintptr_t raw_pmm_allocate_page() {
    // TODO:Check free stack
    uintptr_t ret = pmm_first_free_page;
    if (pmm_first_free_page == pmm_last_page) {
        panic("pmm: OOM  All pages in use");
    }
    pmm_first_free_page += 0x1000;

    return ret;
}

uintptr_t pmm_allocate_page() {
    await_mutex(&pmm_lock);
    uintptr_t page = raw_pmm_allocate_page();
    release_mutex(&pmm_lock);
    return page;
}

uintptr_t pmm_allocate_contiguous(int count) {
    // TODO: if I ever use free lists this will need to change
    
    await_mutex(&pmm_lock);

    uintptr_t page1 = raw_pmm_allocate_page();

    for (int i=0; i<count; i++) {
        raw_pmm_allocate_page();
    }

    release_mutex(&pmm_lock);
    return page1;
}

void pmm_free_page(uintptr_t page) {

    // Add to free stack

}

/*
 * Physical memory map
 *
 * an array representing each physical page in the system so I can track
 * refcounts in from the virtual memory system and some metadata
 *
 *
 * NEW IDEA septmeber 2018:
 * vector<uint16_t> with no special cases
 * fork() increfs everything userspace
 * COW decrefs old page and creates new page @ 0
 */

uint16_t *pmm_memory_map;
size_t pmm_memory_map_len = 0;

#define __ng_pmm_map_debug

#define PMM_MAP_DEBUG_CHECK \
    do { \
        assert(cell > 0 && cell <= pmm_memory_map_len, ""); \
        assert(pmm_memory_map[cell] != PMM_MAP_NOPHY, ""); \
    } while (0)

#ifdef __ng_pmm_map_debug
# define _ng_pmm_map_inline
  const bool pmm_map_debug = true;
#else
# define _ng_pmm_map_inline inline
  const bool pmm_map_debug = false;
#endif


_ng_pmm_map_inline
void pmm_settype(uintptr_t pma, int type) {
    size_t cell = pma / 4096;

    if (pmm_map_debug)
        PMM_MAP_DEBUG_CHECK;

    pmm_memory_map[cell] &= 0x0fff;
    pmm_memory_map[cell] |= type;
}

_ng_pmm_map_inline
void pmm_setref(uintptr_t pma, int refcnt) {
    size_t cell = pma / 4096;

    if (pmm_map_debug)
        PMM_MAP_DEBUG_CHECK;

    pmm_memory_map[cell] &= 0xf000;
    pmm_memory_map[cell] |= refcnt;
}

_ng_pmm_map_inline
int pmm_type(uintptr_t pma) {
    size_t cell = pma / 4096;

    if (pmm_map_debug)
        PMM_MAP_DEBUG_CHECK;

    return pmm_memory_map[cell] & 0xf000;
}

_ng_pmm_map_inline
int pmm_getref(uintptr_t pma) {
    size_t cell = pma / 4096;

    if (pmm_map_debug)
        PMM_MAP_DEBUG_CHECK;

    return pmm_memory_map[cell] & 0x0fff;
}

_ng_pmm_map_inline
int pmm_incref(uintptr_t pma) {
    size_t cell = pma / 4096;

    if (pmm_map_debug) {
        PMM_MAP_DEBUG_CHECK;
        assert(pmm_getref(pma) < 0xfff, "");
    }

    return pmm_memory_map[cell] += 1;
}

_ng_pmm_map_inline
int pmm_decref(uintptr_t pma) {
    size_t cell = pma / 4096;

    if (pmm_map_debug) {
        PMM_MAP_DEBUG_CHECK;
        assert(pmm_getref(pma) > 0, "");
    }

    return pmm_memory_map[cell] -= 1;
}

//
// TODO: figure out where and how to set the initial mapping on this
//
// Thoughts:
//
// what is a refernce?  just a virtual meory mapping or using the memory
// at boot there are meory mapping that no one is using
// should I remove these?
// should I could these?
// I then use them for handing out malloc space initally
// could I just go full vmm_create_unbacked on it all?
// how do I know with sufficient confidence that physical memory including
//  a) the kernel and paging structures
//  b) the initfs
//  and
//  c) any devices I need
// is 100% mapped if I'm trying to be frugal
//
//

