
#include <stdlib.h>

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

div_t div(int x, int y) { return (div_t){.quot = x / y, .rem = x % y}; }

ldiv_t ldiv(long x, long y) { return (ldiv_t){.quot = x / y, .rem = x % y}; }

lldiv_t lldiv(long long x, long long y) {
        return (lldiv_t){.quot = x / y, .rem = x % y};
}

char *getenv(const char *name) { return ""; }
