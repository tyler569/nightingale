#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __kernel__

int abs(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

long labs(long x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

long long llabs(long long x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

div_t div(int x, int y) {
    return (div_t){.quot = x / y, .rem = x % y};
}

ldiv_t ldiv(long x, long y) {
    return (ldiv_t){.quot = x / y, .rem = x % y};
}

lldiv_t lldiv(long long x, long long y) {
    return (lldiv_t){.quot = x / y, .rem = x % y};
}

char *getenv(const char *name) {
    return "";
}

void abort(void) {
    exit(1);
}

long int random_state = 0x1478123;

long int random(void) {
    random_state *= 4784723894;
    random_state /= 7832;
    random_state <<= 7;
    random_state ^= 0x727284849291ADDF;
    random_state *= random_state % 8797423;

    return random_state;
}

void srandom(unsigned seed) {
    random_state = seed;
}

void (*atexit_functions[ATEXIT_MAX])(void);
int atexit_count;

int atexit(void (*fn)(void)) {
    if (atexit_count == ATEXIT_MAX) {
        fprintf(stderr, "atexit: too many functions registered\n");
        return 0;
    }
    atexit_functions[atexit_count++] = fn;
    return 0;
}

void exit(int status) {
    for (int i = atexit_count - 1; i >= 0; i--) { atexit_functions[i](); }
    _exit(status);
}

#endif // ifndef __kernel__

long int strtol(const char *nptr, char **endptr, int base) {
    assert(base <= 10);

    int index = 0;
    long value = 0;
    int sign = 1;

    if (nptr[0] == '-') {
        sign = -1;
        index += 1;
    }
    if (nptr[0] == '+') {
        index += 1;
    }
    while (isdigit(nptr[index])) {
        value *= base;
        value += nptr[index] - '0';
        index += 1;
    }

    if (endptr) *endptr = (char *)nptr + index;
    return sign * value;
}

long long int strtoll(const char *nptr, char **endptr, int base) {
    return strtol(nptr, endptr, base);
}

unsigned long strtoul(const char *nptr, char **endptr, int base) {
    assert(base == 10);

    int index = 0;
    long value = 0;
    int sign = 1;

    while (isdigit(nptr[index])) {
        value *= base;
        value += nptr[index] - '0';
        index += 1;
    }

    if (endptr) *endptr = (char *)nptr + index;
    return sign * value;
}

int atoi(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

long atol(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

long long atoll(const char *nptr) {
    return strtoll(nptr, NULL, 10);
}

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *)) {
    return;
}
