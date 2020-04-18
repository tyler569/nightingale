#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include "list.h"

/*
TODO list
 
should implement in list:
list_sort(list *head, int ordering(void *, void *));

*/


/*
TODOs and open questions:

- how do bufctl's actually work? Do I need to have them allocated when objects
are in use or not?
- how do I map an object back to a slab/bufctl when it is freed?

*/

/*
Essentially the overall design:

users request an "allocator" with a size and a contructor method. This creates
some number of objects pre-constructed for use.

The interesting part of this system is the 2-level free list approach to
managing free buffers. Honestly I could loose the "constructed" notion for
now, I'm a lot less interested in troubleshooting that tbh.

Allocators own 0..many "slabs", which are regions of contiguous virtual memory
that contain "buffers" or designated slots for objects to be allocated in.

For small object sizes (<256 probably), a "slab" is just one page, for larger
object sizes it is larger, chosen to minimize loss from things not fitting
well in the slab.

Allocators will try to provide objects from the most full slabs first, to
minimize fragmentation across slabs, and to try to keep as many slabs as
possible completely empty -- these can be reclaimed by the system in the
event of memory pressure.

Slabs maintain a free list of empty buffers, along with metadata about the
contents of the slab.

*/


struct mem_slab;
struct mem_cache;
struct bufctl;
struct small_bufctl;

typedef struct mem_cache mem_cache;

// Nodes in small buffers - as little as 8 bytes.
struct small_bufctl {
    struct small_bufctl *free_next;
};

struct mem_slab {
    struct mem_cache *allocator;
    int color; // TODO
    int redzone;
    
    int free_count;

    union {
        // large objects
        struct {
            list free_buffers;
            list all_buffers;
        }
        // small objects
        struct small_bufctl *free_head;
    }

    list_node slabs;
    list_node partial_slabs;
    list_node free_slab;

    void *buffer;
    size_t buffer_length;
};

struct mem_cache {
    char *name;
    int object_size;
    int object_align;
    void (*constructor)(void *, size_t);
    void (*destructor)(void *, size_t);

    list slabs;
    list partial_slabs;
    list free_slabs;

    int slab_count;
    int object_count;
    int objects_per_slab;
    int objects_freed_lifetime;
    int objects_allocated_lifetime;

    list_node caches;
};

void *mem_cache_alloc(struct mem_cache *allocator);
void mem_cache_free(struct mem_cache *allocator, void *object);

void mem_cache_grow(struct mem_cache *allocator);
void mem_cache_reap(struct mem_cache *allocator);

void *vm_page_alloc(int count) {
#define PAGE_SIZE 4096
    return malloc(count * PAGE_SIZE);
}


#define SLAB_SMALL_THRESHOLD 512
int slab_is_small(struct mem_slab *sl) {
    return sl->allocator->object_size < SLAB_SMALL_THRESHOLD;
}

int slab_is_external(struct mem_slab *sl) {
    return sl->allocator->object_size > 256;
}

int slab_contains(struct mem_slab *sl, void *object) {
    // TODO: check if the object is valid
    
    if (slab_is_small(sl)) {
        // iterate ets
    } else {
        // iterate etc
    }

    return sl->object >= sl->buffer &&
        sl->object < sl->buffer + sl->buffer_length;
}



void *mem_cache_alloc(struct mem_cache *allocator) {
    struct mem_slab *sl;

    if (list_head(&allocator->partial_slabs)) {
        sl = list_head_entry(&allocator->partial_slabs);
    } else if (list_head(&allocator->free_slabs)) {
        sl = list_head_entry(&allocator->free_slabs);
    } else {
        mem_cache_grow(allocator);
        sl = list_head_entry(&allocator->free_slabs);
    }

    // LOCK sl
    sl->free_count -= 1;
    struct bufctl *bc = list_pop_front(struct bufctl, &sl->free_buffers, free_buffers);
    // UNLOCK sl

    return bc->buffer;
}

void mem_cache_free(struct mem_cache *allocator, void *object) {
    struct mem_slab *sl;
    bool found = false;
    list_foreach(&allocator->slabs, sl, slabs) {
        if (slab_contains(sl, object)) {
            found = true;
            break;
        }
    }

    if (!found) {
        printf("invalid free -- %p is not in this allocator\n", object);
    }

    if (slab_is_small(sl)) {
    } else {
    }

    sl->free_count += 1;
    
    // on some cadence:
    // list_sort(&allocator->slabs, slabs, slots_filled)
}


struct example {
    int x, y, z;
};

void example_constructor(void *object, size_t length) {}
void example_destructor(void *object, size_t length) {}

int main() {
    mem_cache *c = mem_cache_new(
            struct example, 
            example_constructor,
            example_destructor
    );

    struct example *e = mem_cache_alloc(c);
    e->x = 10;
    e->y = 11;
    e->z = 12;
    mem_cache_free(c, e);
}
