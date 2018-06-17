
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argv) {
        while (*argv) {
            printf("%s ", *argv);
            argv++;
        }
    }
    printf("\n");
}

void _start(int argc, char **argv, char **envp) {
    int exit_status = main(argc, argv);
    exit(exit_status);
}

