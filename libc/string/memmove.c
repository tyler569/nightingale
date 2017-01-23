
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void *memmove(void *destx, const void *srcx, size_t len) {
    uint8_t *dest = (uint8_t *)destx;
    const uint8_t *src = (const uint8_t *)srcx;

    for (size_t i=0; i<len; i++)
        dest[i] = src[i];

    return destx;
}

