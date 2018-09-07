
#ifndef NIGHTINGALE_VECTOR_H
#define NIGHTINGALE_VECTOR_H

struct vector {
    const char *type;
    size_t len;
    size_t total_size;
    size_t delta;
    void *data;
};

struct vector *new_vec_internal(struct vector *v, const char *type, size_t count, size_t delta);

#define vec_init(vec, type) new_vec_internal(vec, #type, 16, sizeof(type))

void vec_set(struct vector *vec, size_t index, void *value);
size_t vec_push(struct vector *vec, void *value);
void *vec_get(struct vector *vec, size_t index);

void vec_set_value(struct vector*, size_t, uintptr_t);
size_t vec_push_value(struct vector*, uintptr_t);
uintptr_t vec_get_value(struct vector*, size_t);

size_t vec_init_copy(struct vector*, struct vector* source);

void print_vector(struct vector *);

#endif
