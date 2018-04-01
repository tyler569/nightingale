
#ifndef NIGHTINGALE_VECTOR_H
#define NIGHTINGALE_VECTOR_H

enum strategy {
    STRATEGY_BY_1,
    STRATEGY_BY_16,
    STRATEGY_BY_256,
    STRATEGY_PHI,
    STRATEGY_X2,
    STRATEGY_X4,
};

struct vector {
    const char *type;
    size_t len;
    size_t total_size;
    size_t delta;
    int strategy;
    void *data;
};

struct vector *new_vec_internal(const char *type, size_t count, size_t delta, int strategy);
#define new_vec(type) new_vec_internal(#type, 16, sizeof(type), STRATEGY_PHI)
#define new_vec_s(type, strat) new_vec_internal(#type, 16, sizeof(type), strat)

void vec_set(struct vector *vec, size_t index, void *value);
void vec_push(struct vector *vec, void *value);
void *vec_get(struct vector *vec, size_t index);

void print_vector(struct vector *);

#endif
