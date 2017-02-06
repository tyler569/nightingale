
/*
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <kernel/printk.h>


__attribute__((__noreturn__))
void abort() {
    printk("abort() -> HALT\n");

    __asm__ ("cli");
    for (;;)
        __asm__ ("hlt");
    __builtin_unreachable();
}

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

void *memcpy(void *restrict destx, const void *restrict srcx, size_t size) {
    uint8_t *dest = (uint8_t *)destx;
    const uint8_t *src = (const uint8_t *)srcx;

    for (size_t i=0; i<size; i++)
        dest[i] = src[i];

    return destx;
}

void *memmove(void *destx, const void *srcx, size_t len) {
    uint8_t *dest = (uint8_t *)destx;
    const uint8_t *src = (const uint8_t *)srcx;

    for (size_t i=0; i<len; i++)
        dest[i] = src[i];

    return destx;
}

void *memset(void *destx, int c, size_t size) {
    uint8_t *dest = (uint8_t *)destx;

    for (size_t i=0; i<size; i++) {
        dest[i] = (uint8_t)c;
    }
    return destx;
}

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len++]);
    return len - 1;
}


