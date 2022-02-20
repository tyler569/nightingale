#include <stdio.h>
#include <stdlib.h>

int main() {
    for (size_t i = 1; i != 0; i <<= 1) {
        printf("allocating %zu\n", i);
        char *x = malloc(i);
        for (size_t n = 0; n < i; n++) {
            x[n] = 1;
        }
    }
    return 0;
}
