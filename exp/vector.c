
/*
#include <malloc.h>
#include <string.h>
#include <print.h>
#include <panic.h>
#include "vector.h"
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum {
    VECTOR_NORMAL,
    VECTOR_TRACKED,
};

struct vector {
    const char* type;
    size_t len;
    size_t total_size;
    size_t delta;
    char* data;
};

struct vector* new_vec_internal(
        struct vector* result, const char *type, size_t count, size_t delta) {
    result->type = type;
    result->vec_type = VECTOR_NORMAL;
    result->len = 0;
    result->total_size = count;
    result->delta = delta;
    result->data = malloc(count * delta);

    return result;
}

size_t vec_init_copy(struct vector* vec, struct vector* source) {
    memcpy(vec, source, sizeof(struct vector));
    vec->data = malloc(source->total_size * source->delta);
    memcpy(vec->data, source->data, source->total_size * source->delta);
    return vec->len;
}

size_t vec_expand(struct vector* vec, size_t new_len) {
    printf("expanding ");
    print_vector(vec);

    if (new_len < vec->len)  return vec->len; // no shrinking

    char* new_data;
    new_data = realloc(vec->data, new_len * vec->delta);
    assert(new_data != NULL, "Reallocating to up buffer failed");

    vec->total_size = new_len;
    vec->data = new_data;
    return new_len;
}

void vec_set(struct vector* vec, size_t index, void* value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");
    memcpy((vec->data + (vec->delta * index)), value, vec->delta);
}

size_t vec_push(struct vector* vec, void* value) {
    if (vec->total_size > vec->len) {
        vec_set(vec, vec->len, value);
        vec->len += 1;
        return vec->len - 1;
    } else {
        vec_expand(vec, vec->len * 3/2); // handle error? I assert for now
        return vec_push(vec, value);
    }
}

void* vec_get(struct vector* vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return vec->data + (vec->delta * index);
}

void print_vector(struct vector* v) {
    printf("struct vector<%s> ", v->type);
    printf("@%zx{len=%zu, total_size=%zu, data=%zx}\n", 
        v, v->len, v->total_size, v->data);
}

void vec_set_value(struct vector* vec, size_t index, uintptr_t value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");
    ((uintptr_t*)vec->data)[index] = value;
}

void vec_set_value_ex(struct vector* vec, size_t index, uintptr_t value) {
    while (vec->len < index) {
        vec_expand(vec, vec->len * 3/2);
    }
    ((uintptr_t*)vec->data)[index] = value;
}

size_t vec_push_value(struct vector* vec, uintptr_t value) {
    if (vec->total_size > vec->len) {
        vec_set_value(vec, vec->len, value);
        vec->len += 1;
        return vec->len - 1;
    } else {
        vec_expand(vec, vec->len * 3/2); // handle error? I assert for now
        return vec_push_value(vec, value);
    }
}

uintptr_t vec_get_value(struct vector* vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return ((uintptr_t*)vec->data)[index];
}

void vec_free(struct vector* vec) {
    vec->type = "free";
    vec->len = 0;
    vec->total_size = 0;
    vec->delta = 0;
    free(vec->data);
    vec->data = NULL;
}

//////////////////////////////////////////////////////////////////////////////

struct tvector {
    const char* type;
    // int mutex; <- all internal ops take the mutex before operating
    //               maybe tvec_get doesn't release it automatically
    //               because you have a pointer into the internal memory
    //               if you need to keep your copy around use copyto
    size_t len;
    size_t total_size;
    size_t delta;
    int64_t free_list_index; // -1: none
    char* data;
};

struct tvector* new_tracked_vec_internal(
        struct tvector* result, const char *type, size_t count, size_t delta) {
    result->type = type;
    result->vec_type = tvector_TRACKED;
    result->len = 0;
    result->total_size = count;
    result->delta = delta + sizeof(uint32_t);
    result->free_list_index = -1;
    result->data = malloc(count * delta);

    return result;
}

typedef uint64_t vec_handle;
#define TV_SIG_BITS 32

// maybe random at some point, for now I'm happy with a global incrementing
// value
uint64_t tvector_the_random_value = 0;

int64_t handle_to_index(struct tvector* vec, vec_handle handle) {
    uint64_t handle_sig_mask = ((1 << TV_SIG_BITS) - 1);
    uint64_t candidate_len = handle & handle_sig_mask;
    uint64_t candidate_rnd = handle & ~handle_sig_mask >> TV_SIG_BITS;

    if (candidate_len > vec->len) {
        return -1;
    }

    uint32_t* real_rnd = (uint32_t*)(vec->data + vec->delta * candidate_len);
    if (*real_rnd == candidate_rnd) {
        return (int64_t)candidate_len;
    } else {
        return -1;
    }
}

/*
 * You probably can't init a copy of a tvector, since you wouldn't know
 * the handles into it without inserting, unless I want to just use the
 * existing ones?  I dunno, I think this is better just used for globally
 * shared things that I don't want the tables to grow too large.
 *
 * This requires more thought
 *
size_t tvec_init_copy(struct tvector* vec, struct tvector* source) {
    memcpy(vec, source, sizeof(struct tvector));
    vec->data = malloc(source->total_size * source->delta);
    memcpy(vec->data, source->data, source->total_size * source->delta);
    return vec->len;
}
*/

void print_tvector(struct tvector* v) {
    printf("struct tvector<%s> ", v->type);
    printf("@%zx{len=%zu, total_size=%zu, data=%zx}\n", 
        v, v->len, v->total_size, v->data);
}

size_t tvec_expand_internal(struct tvector* vec, size_t new_len) {
    printf("expanding ");
    print_tvector(vec);

    if (new_len < vec->len)  return vec->len; // no shrinking

    char* new_data;
    new_data = realloc(vec->data, new_len * vec->delta);
    assert(new_data != NULL, "Reallocating to up buffer failed");

    vec->total_size = new_len;
    vec->data = new_data;
    return new_len;
}

void tvec_set_internal(struct tvector* vec, int64_t index, void* value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");

    *(uint32_t*)(vec->data + (vec->delta * index) + 4) =
        tvector_the_random_value;
    memcpy((vec->data + (vec->delta * index) + 4), value, vec->delta);
}

vec_handle tvec_put(struct tvector* vec, void* value) {
    // await_mutex
    if (vec->free_list_index != -1) {
        // do free list things
    }

    if (vec->total_size > vec->len) {
        tvec_set_internal(vec, vec->len, value);
        vec->len += 1;
        return vec->len - 1;
    } else {
        tvec_expand_internal(vec, vec->len * 3/2);
        return tvec_put(vec, value);
    }
    // release_mutex
}

void* tvec_get(struct tvector* vec, vec_handle handle) {
    // await_mutex
    assert(vec->len > index, "Access out-of-bounds");
    int64_t index = handle_to_index(handle);
    if (index < 0) {
        return NULL;
    }
    return vec->data + (vec->delta * index) + 4;
    // you call tvec_release when you're done with the pointer
}

int tvec_copyto(struct tvector* vec, void* data, vec_handle index) {
    // await_mutex
    assert(vec->len > index, "Access out-of-bounds");
    int64_t index = handle_to_index(handle);
    if (index < 0) {
        return NULL;
    }
    memcpy(data, vec->data + (vec->delta * index) + 4, vec->delta - 4);
    // release_mutex
}

void tvec_release(struct tvector* vec) {
    // release_mutex(vec->mutex);
}
