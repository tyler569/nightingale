#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define vector(T) struct __vector_ ## T

#define struct_vector(T) \
        struct __vector_ ## T { \
                T *ptr; \
                size_t len; \
                size_t cap; \
        }; \
        \
        void __vec_init_ ## T(vector(T) *vec) { \
                vec->len = 0; \
                vec->cap = 32; \
                vec->ptr = malloc(sizeof(T) * 32); \
        } \
        T __vec_get_ ## T(vector(T) *vec, size_t off) { \
                return vec->ptr[off]; \
        } \
        T __vec_push_ ## T(vector(T) *vec, T value) { \
                vec->ptr[vec->len] = value; \
                vec->len++; \
        }

#define vec_init(T) __vec_init_ ## T
#define vec_get(T) __vec_get_ ## T
#define vec_push(T) __vec_push_ ## T

struct_vector(int)

vector(int) x;

int main() {
        vec_init(int)(&x);
        vec_push(int)(&x, 100);
        vec_push(int)(&x, 101);
        vec_push(int)(&x, 102);

        for (int i=0; i<x.len; i++) {
                printf("%i\n", vec_get(int)(&x, i));
        }
}

