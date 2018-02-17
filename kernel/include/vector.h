
#ifndef NIGHTINGALE_VECTOR_H
#define NIGHTINGALE_VECTOR_H

typedef enum Strategy {
    STRATEGY_BY_1,
    STRATEGY_BY_16,
    STRATEGY_BY_256,
    STRATEGY_PHI,
    STRATEGY_X2,
    STRATEGY_X4,
} Strategy;

typedef struct Vector {
    const char *type;
    size_t len;
    size_t total_size;
    size_t delta;
    Strategy strategy;
    void *data;
} Vector;

Vector *new_vec_internal(const char *type, size_t count, size_t delta, Strategy strategy);
#define new_vec(type) new_vec_internal(#type, 16, sizeof(type), STRATEGY_PHI)
#define new_vec_s(type, strat) new_vec_internal(#type, 16, sizeof(type), strat)

void vec_set(Vector *vec, size_t index, void *value);
void vec_push(Vector *vec, void *value);
void *vec_get(Vector *vec, size_t index);

void print_vector(Vector *);

#endif
