
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("\n");
}

void _start(char **argv, char **envp) {
    int argc = 0;
    char **argv_c = argv;

    while (*argv_c) {
        argc += 1;
        argv_c += 1;
    }

    int exit_status = main(argc, argv);
    exit(exit_status);
}

