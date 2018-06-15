
#include <basic.h>
#include <string.h>
#include <panic.h>

bool isalnum(char c) {
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'A') && (c <= 'Z')) ||
           ((c >= 'a') && (c <= 'z'));
}

bool isalpha(char c) {
    return ((c >= 'A') && (c <= 'Z')) ||
           ((c >= 'a') && (c <= 'z'));
}

bool islower(char c) {
    return ((c >= 'a') && (c <= 'z'));
}

bool isupper(char c) {
    return ((c >= 'A') && (c <= 'Z'));
}

bool isdigit(char c) {
    return ((c >= '0') && (c <= '9'));
}

bool isxdigit(char c) {
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'a') && (c <= 'f')) ||
           ((c >= 'A') && (c <= 'F'));
}

bool iscntrl(char c) {
    return (c <= 31 || c == 127);
}

bool isspace(char c) {
    return (c >= 9 && c <= 13) || c == ' '; // c in "\t\n\v\f\r "
}

bool isblank(char c) {
    return (c == 9 || c == ' ');
}

bool isprint(char c) {
    return (c > 31 && c < 127);
}

bool ispunct(char c) {
    return (c >= '!' && c <= '/') ||
           (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') ||
           (c >= '{' && c <= '~');
}

char *strcpy(char *dest, char *src) {
    while (*src != 0) {
        *dest++ = *src++;
    }
    *dest = *src; // copy the \0

    return dest;
}

char *strncpy(char *dest, char *src, usize count) {
    int i;
    for (i=0; i<count && *src != 0; i++) {
        *dest++ = *src++;
    }
    if (i < count) {
        *dest = *src; // copy the \0 if there is room left
    }
    return dest;
}

usize strlen(const char *s) {
    usize i = 0;
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

int strncmp(const char *a, const char *b, usize count) {
    for (usize i=0; i<count; i++) {
        if (*a == *b) {
            a++, b++;
            continue;
        }
        return *b - *a;
    }
    return 0;
}

char *strchr(char *s, char c) {
    while (*s != 0) {
        if (*s == c) {
            return s;
        } else {
            s++;
        }
    }
    return NULL;
}

char *strcat(char *restrict dest, const char *restrict src) {
    char *end = dest + strlen(dest);
    size_t len = strlen(src);
    size_t i;

    for (i=0; i<len; i++) {
        end[i] = src[i];
    }

    end[i] = '\0';

    return dest;
}

char *strncat(char *restrict dest, const char *restrict src, size_t max) {
    char *end = dest + strlen(dest);
    size_t len = strlen(src);
    size_t i;

    if (max > len) {
        len = max;
    }

    for (i=0; i<len; i++) {
        end[i] = src[i];
    }

    end[i] = '\0';

    return dest;
}

void *memchr(void *mem_, u8 v, usize count) {
    u8 *mem = mem_;
    for (int i=0; i<count; i++, mem++) {
        if (*mem == v) {
            return mem;
        }
    }
    return NULL;
}

int memcmp(const void *a_, const void *b_, usize count) {
    const u8 *a = a_;
    const u8 *b = b_;
    /*
    for (int i=0; i<count && *a == *b; i++, a++, b++) {
    }
    return *b - *a; // test!
    */

    for (size_t i=0; i<count; i++) {
        if (*a - *b) {
            return *a - *b;
        }
    }
    return 0;
}

void *memset(void *dest_, u8 value, usize count) {
    u8 *dest = dest_;
    for (usize i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *wmemset(void *dest_, u16 value, usize count) {
    u16 *dest = dest_;
    for (usize i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *lmemset(void *dest_, u32 value, usize count) {
    u32 *dest = dest_;
    for (usize i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *qmemset(void *dest_, u64 value, usize count) {
    u64 *dest = dest_;
    for (usize i=0; i<count; i++) {
        dest[i] = value;
    }
    return dest;
}

void *memcpy(void *restrict dest_, const void *restrict src_, usize count) {

    if ((dest_ < src_ && dest_ + count > src_) ||
        (dest_ > src_ && dest_ + count < src_)) {

        panic_bt("overlapping call to memcpy is invalid");
    }
        

    u8 *dest = dest_;
    const u8 *src = src_;

    for (usize i=0; i<count; i++) {
        dest[i] = src[i];
    }

    return dest;
}

void *memmove(void *dest_, const void *src_, usize count) {
    u8 *dest = dest_;
    const u8 *src = src_;

    if (dest < src && dest + count > src) {
        // overlap, dest is lower.
        // no action
    }
    if (dest > src && dest + count < src) {
        // overlap, src is lower.
        // move in reverse
        for (usize i=count-1; i>=0; i--) {
            dest[i] = src[i];
        }
        return dest;
    }

    for (usize i=0; i<count; i++) {
        dest[i] = src[i];
    }

    return dest;
}

