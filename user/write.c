
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv) {
        FILE *s = fopen("foobar", "w");
        if (!s) {
                perror("fopen()");
                return EXIT_FAILURE;
        }
        for (int i=0; i<argc; i++) {
                fprintf(s, "%s\n", argv[i]);
        }

        return EXIT_SUCCESS;
}

