
#pragma once
#ifndef NIGHTINGALE_ALLOCATOR_H
#define NIGHTINGALE_ALLOCATOR_H

#include <basic.h>

void init_physical_allocator(usize first, usize last);
usize phy_allocate_page();
void phy_free_page(usize page);

void heap_init();
void *malloc(usize s);
void free(void *v);

#endif
