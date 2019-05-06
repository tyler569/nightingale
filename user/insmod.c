
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv) {
        if (argc != 2) {
                printf("a file name is required\n");
                exit(EXIT_FAILURE);
        }

        int fd = open(argv[1], O_RDONLY);

        if (fd < 0) {
                perror("open()");
                exit(EXIT_FAILURE);
        }

        printf("Loading %s\n", argv[1]);
        load_module(fd);
        if (errno != SUCCESS) {
                perror("loadmod()");
                exit(EXIT_FAILURE);
        }

        return 0;
}

