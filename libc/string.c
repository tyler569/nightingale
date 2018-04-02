
#include "string.h"

char *strcpy(char *dest, char *src) {
    while (*src != 0) {
        *dest++ = *src++;
    }
    *dest = *src; // copy the \0

    return dest;
}

char *strncpy(char *dest, char *src, size_t count) {
    int i;
    for (i=0; i<count && *src != 0; i++) {
        *dest++ = *src++;
    }
    if (i < count) {
        *dest = *src; // copy the \0 if there is room left
    }
    return dest;
}

size_t strlen(const char *s) {
    size_t i = 0;
    while (*s++ != 0) {
        i++;
    }
    return i;
}

int strcmp(const char *a, const char *b) {
    while (*a == *b) {
        if (*a == 0) {
            return 0;
        }
        a++; b++;
    }
    return *b - *a; // test!
}

int strncmp(const char *a, const char *b, size_t count) {
    for (size_t i=0; i<count; i++) {
        if (*a == *b) {
            a++, b++;
            continue;
        }
        return *b - *a;
    }
    return 0;
}

const char *strchr(const char *s, int c) {
    while (*s != 0) {
        if (*s == c) {
            return s;
        } else {
            s++;
        }
    }
    return NULL;
}

void *memchr(void *mem_, int v, size_t count) {
    unsigned char *mem = mem_;
    for (int i=0; i<count; i++, mem++) {
        if (*mem == v) {
            return mem;
        }
    }
    return NULL;
}

int memcmp(const void *a_, const void *b_, size_t count) {
    const unsigned char *a = a_;
    const unsigned char *b = b_;
    for (int i=0; i<count && *a == *b; i++, a++, b++) {
    }
    return *b - *a; // test!
}

void *memset(void *dest_, unsigned char value, size_t count) {
    unsigned char *dest = dest_;
    for (size_t i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

#ifdef __NG_LIBC_WIDE_MEMSET
void *wmemset(void *dest_, unsigned short value, size_t count) {
    unsigned short *dest = dest_;
    for (size_t i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *lmemset(void *dest_, unsigned int value, size_t count) {
    unsigned *dest = dest_;
    for (size_t i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *qmemset(void *dest_, unsigned long value, size_t count) {
    unsigned long *dest = dest_;
    for (size_t i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}
#endif

void *memcpy(void *restrict dest_, const void *restrict src_, size_t count) {

    unsigned char *dest = dest_;
    const unsigned char *src = src_;

    for (size_t i=0; i<count; i++) {
        dest[i] = src[i];
    }

    return dest;
}

void *memmove(void *dest_, const void *src_, size_t count) {
    unsigned char *dest = dest_;
    const unsigned char *src = src_;

    if (dest > src && dest + count < src) {

        // overlap, src is lower.
        // move in reverse

        for (size_t i=count-1; i>=0; i--) {
            dest[i] = src[i];
        }
        return dest;
    }

    for (size_t i=0; i<count; i++) {
        dest[i] = src[i];
    }

    return dest;
}

