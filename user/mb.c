#include <complex.h>
#include <math.h>
#include <stdio.h>

double complex point(int r, int c) {
    double ph = (double)r / 12.5 - 1.0;
    double pw = (double)c / 30.0 - 0.5;
    return -pw + ph * 1.0di;
}

int mb_iters(double complex z) {
    double complex c = 0;
    for (int i = 0; i < 250; i++) {
        c = c * c + z;
        if (cabs(c) > 10) return i;
    }
    return -1;
}

int ch(int iters) {
    if (iters == -1) return ' ';
    if (iters > 50) return '#';
    if (iters > 25) return 'w';
    if (iters > 10) return 'o';
    if (iters > 5) return '*';
    if (iters > 2) return '.';
    return ' ';
}

int main() {
    for (int r = 0; r <= 25; r++) {
        for (int c = 0; c < 80; c++) {
            int i = mb_iters(point(r, c));
            printf("%c", ch(i));
        }
        printf("\n");
    }
}
