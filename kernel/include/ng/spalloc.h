#pragma once

#include <ng/mem.h>
#include <ng/sync.h>
#include <sys/cdefs.h>
#include <sys/slab.h>
#include <sys/types.h>

BEGIN_DECLS

struct spalloc {
	struct slab_cache cache;
};

#define sp_init(sp, type) _internal_sp_init(sp, sizeof(type));

void _internal_sp_init(struct spalloc *sp, ssize_t object_size);

void *sp_alloc(struct spalloc *sp);
void sp_free(struct spalloc *sp, void *allocation);

END_DECLS
