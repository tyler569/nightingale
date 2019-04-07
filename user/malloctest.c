
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int* foo = malloc(10);
    printf("foo is %p\n", foo);

    int* bar = malloc(100);
    printf("boo is %p\n", foo);

    return 0;
}

