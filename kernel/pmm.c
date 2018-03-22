
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

uintptr_t pmm_allocate_page() {
    await_mutex(&pmm_lock);
   
    // TODO:Check free stack
    uintptr_t ret = pmm_first_free_page;
    if (pmm_first_free_page == pmm_last_page) {
        panic("pmm: OOM  All pages in use");
    }
    pmm_first_free_page += 0x1000;

    return ret;
    release_mutex(&pmm_lock);
}

uintptr_t pmm_allocate_range() {
    // TODO: physical allocate ranges
    return 0;
}

void pmm_free_page(uintptr_t page) {

    // Add to free stack

}

