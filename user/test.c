
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Hello World!\n");
    printf("This is a test executable that was loaded with fork/exec\n");
    return 0;
}

int _start() {
    int exit_status = main();
    exit(exit_status);
}

