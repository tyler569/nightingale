
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
        int fd;
        if (argc == 1) {
                fd = open(".", O_RDONLY);
        } else {
                fd = open(argv[1], O_RDONLY);
        }

        if (fd < 0) {
                perror("open()");
                return EXIT_FAILURE;
        }

        struct ng_dirent dirent_buf[64];

        int entries = getdirents(fd, dirent_buf, 64);

        for (int i=0; i<entries; i++) {
                printf("%s %i %o\n", dirent_buf[i].filename, dirent_buf[i].type, dirent_buf[i].permissions);
        }

        return EXIT_SUCCESS;
}

