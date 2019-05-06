
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
        char buffer[100000];
        while (argc--) {
                fprintf(stdout, "%s\n", argv[argc]);
                char *next = fgets(buffer, 1000, stdin);

                fprintf(stdout, "%s\n", buffer);
        }

        return EXIT_SUCCESS;
}

