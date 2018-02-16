
#include <basic.h>

#define DEBUG
#include <debug.h>
#include <panic.h>

#include "pmm.h"

usize pmm_first_free_page;
usize pmm_last_page;
usize pmm_free_stack_size = 0;

/*
 *
 * TODO
 *
 * free stack
 *
 */

void pmm_allocator_init(usize first, usize last) {
    pmm_first_free_page = first;
    pmm_last_page = last;

    // Set up free stack
    
}

usize pmm_allocate_page() {

    // Check free stack
   
    usize ret = pmm_first_free_page;

    if (pmm_first_free_page == pmm_last_page) {
        panic("pmm: OOM  All pages in use");
    }

#if 0 // Tshoot PMM not failing
    if (pmm_first_free_page > 0x3c00000) { 
        printf("Nearing OOM.  At %p of %p\n", pmm_first_free_page, pmm_last_page);
    }
#endif

    pmm_first_free_page += 0x1000;

    return ret;
}

void pmm_free_page(usize page) {

    // Add to free stack

}

