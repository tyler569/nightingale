#include <ng/spalloc.h>

void _internal_sp_init(struct spalloc *sp, ssize_t object_size) {
	init_slab_cache(&sp->cache, object_size);
}

void *sp_alloc(struct spalloc *sp) { return slab_alloc(&sp->cache); }

void sp_free(struct spalloc *sp, void *allocation) {
	return slab_free(&sp->cache, allocation);
}
