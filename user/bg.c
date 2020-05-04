
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
        if (fork()) {
                return EXIT_SUCCESS;
        }

        char buf;
        read(0, &buf, 1);
}
