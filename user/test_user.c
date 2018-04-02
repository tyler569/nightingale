
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Hello World from ring 3!\n");
    printf("Test printf: %i %#010x\n", 10, 0x1234);

    char test[0x41] = { 0 };
    read(3 /* dev_inc */, test, 0x40);

    printf("from my inc char dev: %s\n", test + 0x20);

    return 0;
}

int _start() {
    int status = main();
    exit(status);
}

