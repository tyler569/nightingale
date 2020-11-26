#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void copy(int out, int in) {
#define BUFSZ 4096
    int count;
    char buf[BUFSZ] = {0};

    while ((count = read(in, buf, BUFSZ)) > 0) { write(out, buf, count); }

    if (count < 0) { perror("read()"); }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        copy(STDOUT_FILENO, STDIN_FILENO);
        return EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; i++) {
        int fd;
        if (strcmp(argv[i], "-") == 0) {
            fd = STDIN_FILENO;
        } else {
            fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                perror("open()");
                return EXIT_FAILURE;
            }
        }

        copy(STDOUT_FILENO, fd);

        if (fd > 2) {
            int err = close(fd);
            if (err) perror("close()");
        }
    }
    return EXIT_SUCCESS;
}
