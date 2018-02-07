
#include <basic.h>

#define DEBUG
#include <debug.h>
#include <panic.h>

#include "pmm.h"

usize phy_first_free_page;
usize phy_last_page;
usize phy_free_stack_size = 0;

/*
 *
 * TODO
 *
 * free stack
 *
 */

void phy_allocator_init(usize first, usize last) {
    phy_first_free_page = first;
    phy_last_page = last;

    // Set up free stack
    
}

usize phy_allocate_page() {

    // Check free stack
   
    usize ret = phy_first_free_page;

    if (phy_first_free_page == phy_last_page) {
        panic("pmm: OOM  All pages in use");
    }

#if 0 // Tshoot PMM not failing
    if (phy_first_free_page > 0x3c00000) { 
        printf("Nearing OOM.  At %p of %p\n", phy_first_free_page, phy_last_page);
    }
#endif

    phy_first_free_page += 0x1000;

    return ret;
}

void phy_free_page(usize page) {

    // Add to free stack

}

