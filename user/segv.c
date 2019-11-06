
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void sigsegv_handler(int signal) {
        printf("SIGSEGV handler running\n");
        exit(1);
}

int main(int argc, char **argv) {
        if (argc > 1 && strcmp(argv[1], "no")) {
                // do nothing
        } else {
                signal(SIGSEGV, sigsegv_handler);
        }

        volatile int *x = NULL;
        *x = 10;

        exit(0);
}

