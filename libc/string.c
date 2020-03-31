
#include <nc/string.h>

char *strcpy(char *dest, const char *src) {
        while (*src != 0) {
                *dest++ = *src++;
        }
        *dest = *src; // copy the \0

        return dest;
}

char *strncpy(char *dest, const char *src, size_t count) {
        int i;
        for (i = 0; i < count && *src != 0; i++) {
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
        for (size_t i=0; ; i++) {
                if (a[i] != b[i])  return a[i] - b[i];
                if (a[i] == 0)  return 0;
        }
}

int strncmp(const char *a, const char *b, size_t count) {
        for (size_t i=0; i<count; i++) {
                if (a[i] != b[i])  return a[i] - b[i];
                if (a[i] == 0) return 0;
        }
        return a[count-1] - b[count-1];
}

char *strchr(const char *s, int c) {
        for (size_t i=0; ; i++) {
                if (s[i] == c)  return (char *)s + i;
                if (s[i] == 0)  return 0;
        }
}

char *strrchr(const char *s, int c) {
        size_t len = strlen(s);
        for (ssize_t i=len; i>=0; i--) {
                if (s[i] == c)  return (char *)s + i;
        }
        return NULL;
}

char *strpbrk(const char *s, const char *accept) {
        for (size_t i=0; ; i++) {
                if (s[i] == 0)  return 0;
                if (strchr(accept, s[i]))  return (char *)s + i;
        }
}

char *strstr(const char *s, const char *subs) {
        const char *found = NULL;

        while (1) {
                const char *ss = subs;
                if (*ss == 0) {
                        return (char *)found;
                } else if (*s == 0) {
                        return NULL;
                } else if (*s == *ss) {
                        s += 1;
                        ss += 1;
                } else {
                        s += 1;
                        ss = subs;
                        found = s;
                }
        }
}

void *memchr(void *mem_, int v, size_t count) {
        unsigned char *mem = mem_;
        for (int i = 0; i < count; i++) {
                if (mem[i] == v) {
                        return mem + i;
                }
        }
        return NULL;
}

int memcmp(const void *a_, const void *b_, size_t count) {
        const unsigned char *a = a_;
        const unsigned char *b = b_;
        for (size_t i=0; i < count; i++) {
                if (a[i] != b[i]) {
                        return a[i] - b[i];
                }
        }
        return 0;
        /*
        for (int i = 0; i < count && *a == *b; i++, a++, b++) {
        }
        return *b - *a; // test!
        */
}

void *memset(void *dest_, unsigned char value, size_t count) {
        unsigned char *dest = dest_;
        for (size_t i = 0; i < count; i++) {
                dest[i] = value;
        }
        return dest;
}

#ifdef _NC_WIDE_MEMSET
void *wmemset(void *dest_, unsigned short value, size_t count) {
        unsigned short *dest = dest_;
        for (size_t i = 0; i < count; i++) {
                dest[i] = value;
        }
        return dest;
}

void *lmemset(void *dest_, unsigned int value, size_t count) {
        unsigned *dest = dest_;
        for (size_t i = 0; i < count; i++) {
                dest[i] = value;
        }
        return dest;
}

void *qmemset(void *dest_, unsigned long value, size_t count) {
        unsigned long *dest = dest_;
        for (size_t i = 0; i < count; i++) {
                dest[i] = value;
        }
        return dest;
}
#endif

void *memcpy(void *restrict dest_, const void *restrict src_, size_t count) {
        unsigned char *dest = dest_;
        const unsigned char *src = src_;

        for (size_t i = 0; i < count; i++) {
                dest[i] = src[i];
        }

        return dest;
}

void *memmove(void *dest_, const void *src_, size_t count) {
        unsigned char *dest = dest_;
        const unsigned char *src = src_;

        if (dest > src) {
                // move in reverse
                for (ssize_t i = count - 1; i >= 0; i--) {
                        dest[i] = src[i];
                }
        } else {
                for (size_t i = 0; i < count; i++) {
                        dest[i] = src[i];
                }
        }

        return dest;
}

size_t strspn(const char *str, const char *accept) {
        size_t slen = strlen(str);
        size_t i;
        for (i=0; i<slen; i++) {
                if (strchr(accept, str[i]) == NULL) {
                        break;
                }
        }
        return i;
}

