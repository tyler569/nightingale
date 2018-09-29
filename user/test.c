
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    printf("Hello World!\n");
    printf("This is a test executable that was loaded with fork/exec\n");
    printf("You called this program with %i arguments\n", argc-1);

    printf("executable name: \"%s\"\n", argv[0]);
    for (int i=1; i<argc; i++) {
        printf("  argument %i is \"%s\"\n", i, argv[i]);
    }
    return 0;
}

