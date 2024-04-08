#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

struct vector {
	const char *type;
	size_t len;
	size_t total_size;
	size_t delta;
	char *data;
};

BEGIN_DECLS

struct vector *new_vec_internal(
	struct vector *v, const char *type, size_t count, size_t delta);

#define vec_init(vec, type) new_vec_internal(vec, #type, 16, sizeof(type))

size_t vec_expand(struct vector *, size_t);
void vec_set(struct vector *vec, size_t index, void *value);
size_t vec_push(struct vector *vec, void *value);
void *vec_get(struct vector *vec, size_t index);
void vec_set_value(struct vector *, size_t, uintptr_t);
void vec_set_value_ex(struct vector *, size_t, uintptr_t);
size_t vec_push_value(struct vector *, uintptr_t);
uintptr_t vec_get_value(struct vector *, size_t);
size_t vec_init_copy(struct vector *, struct vector *source);
void vec_free(struct vector *);
void print_vector(struct vector *);

END_DECLS

