#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

typedef int result_t;

typedef struct region region_t;
typedef struct arena arena_t;

struct region {
    list arenas;
    list partial_arenas;
    list free_arenas;

    size_t obj_size;
    size_t obj_align;
};

void *alloc_arena(size_t length) {
    return malloc(length);
}

//void free_arena(void *region, size_t length) {
//    (void) length;
//    free(region);
//}

struct free_object {
    struct free_object *next;
};

struct arena {
    list_node arenas;
    list_node capacity_arenas; // either region->free_arenas or ->partial_arenas
    void *buffer;
    size_t buffer_length;

    int object_slot_count;
    int slots_free;

    size_t padding_length;

    struct free_object *free_head;
};


#define PAGE_SIZE 4096
#define MAX_SINGLE_PAGE_ARENA 512

// NB: this implementation only works if `to` is a power of two
#define ROUND_UP(n, to) ({ \
        __auto_type __n = (n); \
        __auto_type __to = (to); \
        ((__n + __to - 1) & ~(__to - 1)); })

#define ROUND_UP_ARB(n, to) ({ \
        __auto_type __n = (n); \
        __auto_type __to = (to); \
        ((__n + __to - 1) / __to); })

#define PTR_OFFSET(ptr, at) ((void *)((char *)ptr + at))

size_t object_len_to_arena_length(size_t obj_size) {
    if (obj_size <= MAX_SINGLE_PAGE_ARENA) {
        return PAGE_SIZE;
    } else {
        printf("haha todo\n");
        assert(0);
    }
}

// Creates a new arena object, the main point being to allocate the buffer
// and create the arena_t * pointer into that buffer, then put the pointer
// and length in the created arena_t object.
arena_t *arena_create(size_t arena_length) {
    void *arena_buffer = alloc_arena(arena_length);

    size_t arena_ds_offset = arena_length -
        ROUND_UP_ARB(sizeof(arena_t), alignof(arena_t));

    arena_t *arena = PTR_OFFSET(arena_buffer, arena_ds_offset);

    arena->buffer = arena_buffer;
    arena->buffer_length = arena_length;
    arena->free_head = NULL;

    return arena;
}

// Add a newly created arena to a region -- expects valid buffer and length
// and sets up object status, padding, redzoneing, and free lists.
result_t arena_push(region_t *region, arena_t *arena) {
    assert(arena->buffer_length >= 4096);
    assert(arena > arena->buffer);

    size_t obj_offset = region->obj_size;

    size_t possible_objects = 
        (arena->buffer_length - sizeof(struct arena)) /
        obj_offset;

    printf("want to make %zu objects\n", possible_objects);

    arena->object_slot_count = possible_objects;
    arena->slots_free = possible_objects;
    arena->padding_length = 0;

    struct free_object *free_tail = (struct free_object *)arena->buffer;
    free_tail->next = NULL;

    struct free_object *previous = free_tail;

    for (int i=1; i<possible_objects; i++) {
        printf("making an object slot at %p\n", free_tail);
        free_tail = PTR_OFFSET(free_tail, obj_offset);
        free_tail->next = previous;
        previous = free_tail;
    }

    arena->free_head = free_tail;
    
    list_append(&region->arenas, arena, arenas);
    list_append(&region->free_arenas, arena, capacity_arenas);

    return 0;
}

result_t region_init(region_t *region, size_t obj_size) {
    region->obj_size = obj_size;

    size_t arena_length = object_len_to_arena_length(obj_size);

    arena_t *first_arena = arena_create(arena_length);
    arena_push(region, first_arena);

    return 0;
}

result_t region_grow(region_t *region) {
    return 0;
}


void *region_alloc(region_t *region) {
    struct arena *arena;

    if ((arena = list_head_entry(struct arena, &region->partial_arenas, capacity_arenas))) {

    } else if ((arena = list_head_entry(struct arena, &region->free_arenas, capacity_arenas))) {

    } else {
        region_grow(region);
        // TODO
    }

    void *allocation = arena->free_head;
    assert(allocation);
    arena->free_head = arena->free_head->next;

    arena->slots_free--;

    if (arena->slots_free == 0) {
        list_remove(&arena->capacity_arenas);
    } else if (arena->slots_free == arena->object_slot_count - 1) {
        list_remove(&arena->capacity_arenas);
        list_append(&region->partial_arenas, arena, capacity_arenas);
    }

    return allocation;
}

void region_free(region_t *region, void *ptr) {
    struct arena *arena;
    bool found = false;
    list_foreach(&region->arenas, arena, arenas) {
        if (ptr >= arena->buffer && ptr <= arena) {
            found = true;
            break;
        }
    }

    if (!found) {
        // TODO
        assert(0);
    }

    arena->free_head = (struct free_object *)ptr;
    arena->slots_free++;

    if (arena->slots_free == 1) {
        assert(arena->capacity_arenas.next == NULL);
        list_prepend(&region->partial_arenas, arena, capacity_arenas);
    } else if (arena->slots_free == arena->object_slot_count) {
        list_remove(&arena->capacity_arenas);
        list_prepend(&region->free_arenas, arena, capacity_arenas);
    }
}


struct some_test_struct {
    int a, b, c, d;
};

void print_sts(struct some_test_struct *sts) {
    printf("{ a: %i, b: %i, c: %i, d: %i }\n", sts->a, sts->b, sts->c, sts->d);
}


int main() {
    region_t some_region_v;
    region_t *some_region = &some_region_v;

    region_init(some_region, sizeof(struct some_test_struct));

    struct some_test_struct *sts = region_alloc(some_region);
    sts->a = 1;
    sts->b = 2;
    sts->c = 3;
    sts->d = 4;

    print_sts(sts);

}

