
#include <stdio.h>

int main() {
    printf("test that floating point things work\n");
    float x = 1.0;
    float y = 1.5;

    if (x + y == 2.5) {
        printf("adding works probably\n");
    } else {
        printf("welp, that's no good\n");
    }

    if (x * y == 1.5) {
        printf("multiplication works probably\n");
    } else {
        printf("welp, that's no good\n");
    }

    return (int)(1.5f + 2.75f);
}

