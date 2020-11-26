
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int opt;
    int flags = 0;
    char *option = NULL;
    while ((opt = getopt(argc, argv, "abcd:")) != -1) {
        switch (opt) {
        case '?':
            return EXIT_FAILURE;
        case 'a':
            flags |= 1; break;
        case 'b':
            flags |= 2; break;
        case 'c':
            flags |= 4; break;
        case 'd':
            option = optarg;
        }
    }

    printf("%i\n", flags);
    if (option) {
        printf("option: %s\n", option);
    }
}
