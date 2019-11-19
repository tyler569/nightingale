
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nightingale.h>

void usage() {
        printf("usage: crash [option]\n");
        printf("  -s: null deref (userland)\n");
        printf("  -S: null deref (syscall)\n");

        exit(0);
}

int main(int argc, char **argv) {
        if (argc < 2) {
                usage();
        }
        
        if (strcmp(argv[1], "-s") == 0) {
                volatile int *x = 0;
                return *x;
        } else if (strcmp(argv[1], "-S") == 0) {
                fault(NULL_DEREF);
        } else {
                usage();
        }

        return EXIT_SUCCESS;
}

