#pragma once

#include <stdlib.h>
#include <sys/cdefs.h>

#define vec(T) \
	struct { \
		T *data; \
		size_t size; \
		size_t capacity; \
	}

#define vec_free(v) \
	do { \
		free((v)->data); \
		(v)->data = NULL; \
		(v)->size = 0; \
		(v)->capacity = 0; \
	} while (0)

#define vec_push(v, x) \
	do { \
		if ((v)->size == (v)->capacity) { \
			(v)->capacity = (v)->capacity * 3 / 2 + 1; \
			(v)->data \
				= realloc((v)->data, (v)->capacity * sizeof(*(v)->data)); \
		} \
		(v)->data[(v)->size++] = (x); \
	} while (0)

#define vec_pop(v) \
	do { \
		if ((v)->size > 0) { \
			(v)->size--; \
		} \
	} while (0)

#define vec_clear(v) \
	do { \
		(v)->size = 0; \
	} while (0)

#define vec_reserve(v, n) \
	do { \
		if ((v)->capacity < (n)) { \
			(v)->capacity = (n); \
			(v)->data \
				= realloc((v)->data, (v)->capacity * sizeof(*(v)->data)); \
		} \
	} while (0)

#define vec_resize(v, n) \
	do { \
		if ((v)->capacity < (n)) { \
			(v)->capacity = (n); \
			(v)->data \
				= realloc((v)->data, (v)->capacity * sizeof(*(v)->data)); \
		} \
		(v)->size = (n); \
	} while (0)

#define vec_shrink_to_fit(v) \
	do { \
		if ((v)->size < (v)->capacity) { \
			(v)->capacity = (v)->size; \
			(v)->data \
				= realloc((v)->data, (v)->capacity * sizeof(*(v)->data)); \
		} \
	} while (0)

#define vec_at(v, i) ((v)->data[(i)])
#define vec_size(v) ((v)->size)
#define vec_capacity(v) ((v)->capacity)
#define vec_empty(v) ((v)->size == 0)
#define vec_begin(v) ((v)->data)
#define vec_end(v) ((v)->data + (v)->size)

#define vec_foreach(v) \
	for (typeof((v)->data) it = vec_begin(v); it != vec_end(v); it++)

