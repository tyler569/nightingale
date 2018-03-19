
#pragma once
#ifndef NIGHTINGALE_MALLOC_H
#define NIGHTINGALE_MALLOC_H

#include <basic.h>

void *malloc(size_t s);
void *calloc(size_t count, size_t size);
void *realloc(void *v, size_t s);
void free(void *v);

#endif
