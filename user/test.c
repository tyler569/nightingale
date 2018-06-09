
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Hello World from test!\n");
}

int _start() {
    int exit_status = main();
    exit(exit_status);
}

