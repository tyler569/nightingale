
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_one_process() {
}

int is_number(char *str) {
        char *endptr;
        strtol(str, &endptr, 10);
        return str != endptr;
}

int main() {
        int fd = open("/proc", O_RDONLY);
        if (fd < 0) {
                perror("open()");
                return EXIT_FAILURE;
        }

        struct ng_dirent dirent_buf[64];
        int entries = getdirents(fd, dirent_buf, 64);

        if (entries < 0) {
                perror("getdirents()");
                return EXIT_FAILURE;
        }
        close(fd);

        for (int i=0; i<entries; i++) {
                if (!is_number(dirent_buf[i].filename))  continue;

                char fname[32] = "/proc/";
                strcat(fname, dirent_buf[i].filename);
                int fd2 = open(fname, O_RDONLY);

                char buf[256];
                read(fd2, buf, 256);

                printf("%s\n", buf);
                close(fd2);
        }
}

