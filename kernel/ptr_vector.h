
#ifndef NIGHTINGALE_PTR_VECTOR_H
#define NIGHTINGALE_PTR_VECTOR_H

struct ptr_vector {
    const char *type; // debug only?
    bool debug;
    size_t len;
    size_t total_size;
    void **data;
};

void pvec_init(struct ptr_vector *pv);
void pvec_set(struct ptr_vector *vec, size_t index, void *value);
void pvec_push(struct ptr_vector *vec, void *value);
void *pvec_get(struct ptr_vector *vec, size_t index);

void pvec_print(struct ptr_vector *);

#endif
