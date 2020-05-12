
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
        if (fork()) {
                return EXIT_SUCCESS;
        }

        sleep(1000);
}
