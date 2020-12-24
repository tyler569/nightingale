#include <basic.h>
#include <math.h>

double fabs(double v) {
    if (v < 0) {
        return -v;
    }
    return v;
}

double sqrt(double v) {
    asm volatile(
        "sqrtsd %0, %1"
        : "=x"(v)
        : "0"(v)
    );
    return v;
}

double hypot(double a, double b) {
    return sqrt(a * a + b * b);
}

