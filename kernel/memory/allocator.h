
#pragma once
#ifndef NIGHTINGALE_ALLOCATOR_H
#define NIGHTINGALE_ALLOCATOR_H

#include <basic.h>

void heap_init();
void *malloc(usize s);
void free(void *v);

#endif
