
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main() {
    printf("Hello World from %s %i!\n", "ring", 3);

    // do init things

    while (true) {
        char* argv0 = NULL;
        execve("sh", &argv0, NULL);
    }
}

