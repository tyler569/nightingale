
#include <stdint.h>
#include <stddef.h>
#include <string.h>

int memcmp(const void *ax, const void *bx, size_t size) {
    const uint8_t *a = (const uint8_t *)ax;
    const uint8_t *b = (const uint8_t *)bx;

    for (size_t i=0; i<size; i++) {
        if (a[i] > b[i])
            return 1;
        else if (a[i] < b[i])
            return -1;
    }
    return 0;
}
