// #include <complex.h>
#include <stdio.h>

#define complex _Complex

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

double cabs(double complex c) {
    return hypot(creal(c), cimag(c));
}

double complex point(int r, int c) {
    double ph = (double)r / 12.5 - 1.0;
    double pw = (double)c / 40.0 - 0.5;
    return -pw + ph * 1.0di;
}

void print_double(double c) {
    printf("%i.%03i", (int)c, (int)(fabs(c) * 1000) % 1000);
}
void print_pdouble(double c) {
    printf("%+i.%03i", (int)c, (int)(fabs(c) * 1000) % 1000);
}

void print_complex(double complex c) {
    print_double(creal(c));
    print_pdouble(cimag(c));
}

int mb_iters(double complex z) {
    double complex c = 0;
    for (int i = 0; i < 250; i++) {
        c = c * c + z;
        if (cabs(c) > 10) return i;
    }
    return -1;
}

int main() {
    for (int r = 0; r < 25; r++) {
        for (int c = 0; c < 80; c++) {
            int i = mb_iters(point(r, c));
            if (i == -1) printf(" ");
            else printf("%c", i / 10 + 'a');
        }
        printf("\n");
    }

    print_complex(point(0, 0)); printf("\n");
    print_complex(point(25, 80)); printf("\n");
}
