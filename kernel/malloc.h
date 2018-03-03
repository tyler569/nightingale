
#pragma once
#ifndef NIGHTINGALE_MALLOC_H
#define NIGHTINGALE_MALLOC_H

#include <basic.h>

void *malloc(usize s);
void *realloc(void *v, size_t s);
void free(void *v);

#endif
