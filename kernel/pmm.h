
#pragma once
#ifndef NIGHTINGALE_PMM_ALLOC_H
#define NIGHTINGALE_PMM_ALLOC_H

#include <basic.h>

void pmm_allocator_init(usize first, usize last);
usize pmm_allocate_page();
void pmm_free_page(usize vmm);

#endif
