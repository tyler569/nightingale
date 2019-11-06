
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
        FILE *s = fopen("foobar", "w");
        for (int i=0; i<argc; i++) {
                fprintf(s, "%s\n", argv[i]);
        }

        return EXIT_SUCCESS;
}

