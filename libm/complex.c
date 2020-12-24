#include <basic.h>
#include <complex.h>
#include <math.h>

typedef union {
	double complex z;
    struct {
        double real;
        double imag;
    };
} double_complex;

double creal(double complex z) {
    double_complex dc = {.z = z};
    return dc.real;
}

double cimag(double complex z) {
    double_complex dc = {.z = z};
    return dc.imag;
}

double cabs(double complex c) {
    return hypot(creal(c), cimag(c));
}
