
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void *memset(void *destx, int c, size_t size) {
    uint8_t *dest = (uint8_t *)destx;

    for (size_t i=0; i<size; i++) {
        dest[i] = (uint8_t)c;
    }
    return destx;
}
