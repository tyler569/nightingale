
#include <stddef.h>
#include <stdint.h>

int32_t power(int32_t a, int32_t b) {
    int32_t c = 1;
    while (b--) {
        c *= a;
    }
    return c;
}

/*
 * Utilities - from http://www.osdever.net/bkerndev/Docs/creatingmain.htm
 */

void *memcpy(void *dest, const void *src, size_t count) {
    while (count--)
        *(uint8_t *)dest++ = *(uint8_t *)src++;
    return dest;
}

void *memset(void *dest, uint8_t val, size_t count) {
    while (count--)
        *(uint8_t *)dest++ = val;
    return dest;
}

/*
short *wmemset(short *dest, short val, int count) {
    int i = 0;
    while (count--) {
        dest[i++] = val;
    }
    return dest;
}*/

int32_t strlen(const uint8_t *str) {
    int32_t i;
    for (i=0; str[i] != 0; i++);
    return i;
}

uint8_t inportb(uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(uint16_t _port, uint8_t _data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


