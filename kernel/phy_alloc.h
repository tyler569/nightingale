
#pragma once
#ifndef NIGHTINGALE_PHY_ALLOC_H
#define NIGHTINGALE_PHY_ALLOC_H

#include <basic.h>

void phy_allocator_init(usize first, usize last);
usize phy_allocate_page();
void phy_free_page(usize page);

#endif
