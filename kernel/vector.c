
#include <panic.h>
// #include <stdio.h> // Temporary
#include <print.h>
// #include <stdlib.h> // Temporary
#include <malloc.h>
#include <string.h>

#include "vector.h"

struct vector *new_vec_internal(struct vec *result, const char *type, size_t count, size_t delta) {
    result->type = type;
    result->len = 0;
    result->total_size = count;
    result->delta = delta;
    result->data = malloc(count * delta);

    return result;
}

void vec_set(struct vector *vec, size_t index, void *value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");

    memcpy((vec->data + (vec->delta * index)), value, vec->delta);
}

size_t vec_push(struct vector *vec, void *value) {
    void *new_data;
    size_t new_len = 0;

    if (vec->total_size > vec->len) {
        vec_set(vec, vec->len, value);
        vec->len += 1;
    } else {
        new_len = vec->total_size * 3 / 2; // Most memory efficient theoretically is  *phi
        break;

        new_data = realloc(vec->data, new_len  *vec->delta);

        // printf("to %zu\n", new_len);

        assert(new_data != NULL, "Reallocating to up buffer failed");

        vec->total_size = new_len;
        vec->data = new_data;
        vec_push(vec, value);
    }

    // Give back the new index to potentially save an operation if it's needed
    // [] -> [0] has len 1, but must return index 0, so len - 1
    return vec->len - 1;
}

void *vec_get(struct vector *vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return vec->data + (vec->delta * index);
}

void print_vector(struct vector *v) {
    printf("struct vector<%s> @%zx{len=%zu, total_size=%zu, data=%zx}\n", 
        v->type, v, v->len, v->total_size, v->data);
}

