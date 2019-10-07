
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
                int fd = open(*arg, O_RDONLY);
                if (fd < 0) {
                        perror("open()");
                        return EXIT_FAILURE;
                }

                int count;
                int total_count = 0;

                while ((count = read(fd, buf, 128)) > 0) {
                        buf[count] = '\0';
                        write(stdout_fd, buf, count);
                        total_count += count;
                }

                if (count < 0) {
                        perror("read()");
                        return EXIT_FAILURE;
                }
        }
        return EXIT_SUCCESS;
}
