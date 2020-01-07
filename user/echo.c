
#include <stdio.h>

int main(int argc, char **argv) {
        argv += 1; // eat filename
        while (*argv) {
                printf("%s ", *argv);
                argv++;
        }
        printf("\n");
        return 0;
}

