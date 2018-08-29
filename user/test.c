
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    printf("Hello World!\n");
    printf("This is a test executable that was loaded with fork/exec\n");
    printf("You called this program with %i arguments\n", argc);
    for (int i=0; i<argc; i++) {
        printf("  argument %i is \"%s\"\n", i+1, argv[i]);
    }
    return 0;
}

int _start(int argc, char **argv) {
    int exit_status = main(argc, argv);
    exit(exit_status);
}

