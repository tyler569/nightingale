
#include <basic.h>
#include <ng/panic.h>
#include <ng/vmm.h>
#include <ng/spalloc.h>
#include <nc/stdio.h>
#include <nc/string.h>

void _internal_sp_init(struct spalloc *sp, ssize_t object_size,
                       ssize_t capacity, const char *type_name) {
        sp->object_size = object_size >= sizeof(void *) ?
                              object_size :
                              sizeof(void *);
        sp->region = vmm_reserve(capacity * object_size);
        sp->first_free = NULL;
        sp->bump_free = sp->region;
        sp->count = 0;
        sp->capacity = capacity;
        sp->type_name = type_name;
}

void *sp_alloc(struct spalloc *sp) {
        sp->count += 1;
        if (sp->count == sp->capacity) {
                panic_bt("sp_alloc exhausted - implement next region!\n");
        }

        void *allocation = sp->first_free;
        if (allocation) {
                sp->first_free = *(void **)allocation;
                return allocation;
        }

        if (sp->bump_free == sp_add(sp, sp->region, sp->count)) {
                panic_bt("sp_alloc exhausted IN ERROR - implement next region!\n");
        }

        allocation = sp->bump_free;
        sp->bump_free = sp_inc(sp, sp->bump_free);
        return allocation;
}

void sp_free(struct spalloc *sp, void *allocation) {
        sp->count -= 1;
        memset(allocation, 'G', sp->object_size);
        *(void **)allocation = sp->first_free;
        sp->first_free = allocation;
}

