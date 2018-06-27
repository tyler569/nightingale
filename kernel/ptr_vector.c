
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <panic.h>
#include <print.h>
#include <malloc.h>
#include <string.h>

#include "ptr_vector.h"

// Initialize in-place because sometimes you want this on the stack and
// other times on the heap.  I don't judge
void pvec_init(struct ptr_vector *pv) {
    pv->type = "not right now";
    pv->debug = true;
    pv->len = 0;
    pv->total_size = 32;
    // I do mangae this memory for you though
    pv->data = malloc(sizeof(void *) * 32);
}

void pvec_set(struct ptr_vector *pv, size_t index, void *value) {
    if (pv->debug && index >= pv->len) {
        panic("out of range access to ptr_vector@%#lx - tried %lu, size is %u\n", pv, index, pv->len);
    }

    // ptr_vector is better becasue this isn't a memcpy
    pv->data[index] = value;
}

void pvec_push(struct ptr_vector *pv, void *value) {
    if (pv->len == pv->total_size) {
        // only provide the phi strategy.  It's better anyway
        void **tmp = realloc(pv->data, (pv->total_size * 3) / 2);
        if (tmp == NULL) {
            panic("allocation failed in ptr_vector@pv\n");
        } else {
            pv->data = tmp;
        }
    }
    pv->data[pv->len] = value;
}

void *pvec_get(struct ptr_vector *pv, size_t index) {
    if (pv->debug && index >= pv->len) {
        panic("out of range access to ptr_vector@%#lx - tried %lu, size is %u\n", pv, index, pv->len);
    }

    return pv->data[index];
}

void pvec_print(struct ptr_vector *pv) {
    printf("tbf\n");
}

