
#include <panic.h>
// #include <stdio.h> // Temporary
#include <print.h>
// #include <stdlib.h> // Temporary
#include <malloc.h>
#include <string.h>

#include "vector.h"

struct vector* new_vec_internal(struct vector* result, const char *type, size_t count, size_t delta) {
    result->type = type;
    result->len = 0;
    result->total_size = count;
    result->delta = delta;
    result->data = malloc(count * delta);

    return result;
}

size_t vec_init_copy(struct vector* vec, struct vector* source) {
    memcpy(vec, source, sizeof(*source));
    vec->data = malloc(source->total_size * source->delta);
    memcpy(vec->data, source->data, source->total_size * source->delta);
    return vec->len;
}

static size_t vec_try_expand(struct vector* vec) {
    assert(vec->total_size == vec->len, "Vectors can only expand when they are full");
    void* new_data;
    printf("trying to reallocate vector<%s> with total: %lu, len: %lu\n", vec->type, vec->total_size, vec->len);
    size_t new_len = vec->total_size * 3 / 2; // Most memory efficient theoretically is *phi

    new_data = realloc(vec->data, new_len * vec->delta);
    assert(new_data != NULL, "Reallocating to up buffer failed");

    vec->total_size = new_len;
    vec->data = new_data;
    return new_len;
}

size_t vec_expand(struct vector* vec, size_t new_len) {
    if (new_len < vec->len)  return vec->len; // this function doesn't allow shrinking
    void* new_data;
    printf("trying to reallocate vector<%s> with total: %lu, len: %lu\n", vec->type, vec->total_size, vec->len);
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
        vec_try_expand(vec); // handle error? I assert for now
        return vec_push(vec, value);
    }
}

void* vec_get(struct vector* vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return vec->data + (vec->delta * index);
}

void print_vector(struct vector* v) {
    printf("struct vector<%s> @%zx{len=%zu, total_size=%zu, data=%zx}\n", 
        v->type, v, v->len, v->total_size, v->data);
}

void vec_set_value(struct vector* vec, size_t index, uintptr_t value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");
    ((uintptr_t*)vec->data)[index] = value;
}

void vec_set_value_ex(struct vector* vec, size_t index, uintptr_t value) {
    vec_expand(vec, index + 1);
    ((uintptr_t*)vec->data)[index] = value;
}

size_t vec_push_value(struct vector* vec, uintptr_t value) {
    if (vec->total_size > vec->len) {
        vec_set_value(vec, vec->len, value);
        vec->len += 1;
        return vec->len - 1;
    } else {
        vec_try_expand(vec); // handle error? I assert for now
        return vec_push_value(vec, value);
    }
}

uintptr_t vec_get_value(struct vector* vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return ((uintptr_t*)vec->data)[index];
}

