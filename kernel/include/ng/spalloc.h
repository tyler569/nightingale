#pragma once

#include <ng/sync.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct spalloc {
	ssize_t object_size;
	const char *type_name;
	void *region;
	void *first_free;
	void *bump_free;
	ssize_t count;
	ssize_t capacity;
	spin_lock_t lock;
};

#define sp_at(sp, index) \
	(void *)((char *)(sp)->region + index * (sp)->object_size)
#define sp_add(sp, v, x) (void *)((char *)(v) + (x) * (sp)->object_size)
#define sp_inc(sp, v) sp_add(sp, v, 1)

#define sp_init(sp, type) _internal_sp_init(sp, sizeof(type), 0x10000, #type);

void _internal_sp_init(struct spalloc *sp, ssize_t object_size,
	ssize_t capacity, const char *type_name);

void *sp_alloc(struct spalloc *sp);
void sp_free(struct spalloc *sp, void *allocation);

END_DECLS
