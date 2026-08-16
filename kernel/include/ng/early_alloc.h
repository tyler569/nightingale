#pragma once

#include <stddef.h>

void *early_alloc(size_t len);
void *early_alloc_aligned(size_t len, size_t alignment);
