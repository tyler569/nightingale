
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

const char filetype_sigils[] = {
        [FT_DIRECTORY]  = '/',
        [FT_BUFFER]     = ' ',
        [FT_CHARDEV]    = '^',
        [FT_TTY]        = '#',
        [FT_SOCKET]     = ':',
        [FT_PIPE]       = '&',
        [FT_PROC]       = '%',
};

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
                printf("%s", dirent_buf[i].filename);
                int type = dirent_buf[i].type;
                int perm = dirent_buf[i].permissions;
                if (type == FT_BUFFER && (perm & USR_EXEC)) {
                        printf("*");
                } else {
                        printf("%c", filetype_sigils[type]);
                }
                printf("\n");
                // filetype_sigils[dirent_buf[i].type]);
                //, dirent_buf[i].permissions);
        }

        return EXIT_SUCCESS;
}

