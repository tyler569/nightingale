#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int child = fork();

    if (child) {
        printf("bg: child is %i\n", child);
        return EXIT_SUCCESS;
    }

    while (1) { sleep(1); }
}
