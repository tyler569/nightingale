#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (!argv[1]) {
        fprintf(stderr, "No command specified\n");
        exit(EXIT_FAILURE);
    }

    strace(1);
    return execve(argv[1], argv + 1, NULL);
}
