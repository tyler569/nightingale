#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "slab.h"

int main() {
	validate_page_sizes();

	struct slab_cache cache;
	slab_cache_init(&cache, 8);

	void *foo = slab_alloc(&cache);

	printf("%p\n", foo);
}
