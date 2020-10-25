
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
        int *foo = malloc(10);
        printf("foo is %p\n", (void *)foo);

        int *bar = malloc(100);
        printf("boo is %p\n", (void *)foo);

        return 0;
}
