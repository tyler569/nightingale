
#include <basic.h>

#define DEBUG
#include <debug.h>
#include <panic.h>

#include "phy_alloc.h"

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
        panic("All pages are in use, and we do not reuse free pages yet");
    }
    phy_first_free_page += 0x1000;

    return ret;
}

void phy_free_page(usize page) {

    // Add to free stack

}

