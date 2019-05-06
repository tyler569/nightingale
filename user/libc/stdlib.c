
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
#if X86_64
        random_state ^= 0x727284849291ADDF;
#elif I686
        random_state ^= 0x7272848;
#endif
        random_state *= random_state % 8797423;

        return random_state;
}

void srandom(unsigned seed) {
        random_state = seed;
}

