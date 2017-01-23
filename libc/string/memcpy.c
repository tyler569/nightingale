
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void *memcpy(void *restrict destx, const void *restrict srcx, size_t size) {
    uint8_t *dest = (uint8_t *)destx;
    const uint8_t *src = (const uint8_t *)srcx;

    for (size_t i=0; i<size; i++)
        dest[i] = src[i];

    return destx;
}
