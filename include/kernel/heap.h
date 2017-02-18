
#pragma once

#include <stddef.h>
#include <multiboot.h>

void heap_init();

void *heap_alloc(size_t size);
void heap_free(void *memory);


