
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("This is a test message\n");
    printf("Test printf: %i %#010x\n", 10, 0x1234);

    char test[100];
    read(3 /* dev_inc */, test, 100);

    printf("%s\n", test + 0x20);

    return 0;
}

int _start() {
    int status = main();
    exit(status);
}

