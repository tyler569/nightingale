
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void exit_fn(void) {
    printf("Hello World!\n");
}

void sigint_handler(int x) {
    printf("this is the signal handler\n");
}

int main() {
    atexit(exit_fn);
    signal(SIGINT, sigint_handler);

    while (!feof(stdin)) {
        fgetc(stdin);
    }

    printf("got EOF, exitting\n");
}

