#pragma once

#include "list.h"

struct slab_cache {
	struct list_head slabs;
	size_t object_size;
	size_t requested_size;
};

void slab_cache_init(struct slab_cache *cache, size_t requested_size);

void *slab_alloc(struct slab_cache *cache);
void slab_free(struct slab_cache *cache, void *ptr);
void validate_page_sizes();
