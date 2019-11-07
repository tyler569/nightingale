
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
        char buf[128] = {0};

        for (char **arg = argv + 1; *arg; arg++) {
                int fd;
                if (strcmp(*arg, "-") == 0) {
                        fd = STDIN_FILENO;
                } else {
                        fd = open(*arg, O_RDONLY);
                        if (fd < 0) {
                                perror("open()");
                                return EXIT_FAILURE;
                        }
                }

                int count;

                while ((count = read(fd, buf, 128)) > 0) {
                        write(STDOUT_FILENO, buf, count);
                }

                if (count < 0) {
                        perror("read()");
                        return EXIT_FAILURE;
                }

                if (fd > 2) {
                        int err = close(fd);
                        if (err)  perror("close()");
                }
        }
        return EXIT_SUCCESS;
}

