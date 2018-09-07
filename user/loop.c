
#include <stdio.h>

int main() {
    for (long i=0; i<10000000; i++) {
        if (i % 1000000 == 0) {
            printf("i is %li\n", i);
        }
    }
    return 0;
}

