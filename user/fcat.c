
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
        char buf[129] = {0};

        for (char **arg = argv + 1; *arg; arg++) {
                FILE *file = fopen(*arg, "r");

                char buf[4096];

                while (!feof(file)) {
                        fgets(buf, 4096, file);
                        printf("%s", buf);
                }
        }
        return EXIT_SUCCESS;
}
