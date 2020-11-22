#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void check_err(int code, const char *message) {
    if (code < 0) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

const char filetype_sigils[] = {
    [FT_DIRECTORY] = '/', [FT_BUFFER] = ' ', [FT_CHARDEV] = '^', [FT_TTY] = '#',
    [FT_SOCKET] = ':',    [FT_PIPE] = '&',   [FT_PROC] = '%',
};

char ft_sigil(struct ng_dirent *dirent) {
    int type = dirent->type;
    int perm = dirent->permissions;
    if (type == FT_BUFFER && (perm & USR_EXEC)) {
        return '*';
    } else {
        return filetype_sigils[type];
    }
}

int main(int argc, char **argv) {
    int ttyout = isatty(STDOUT_FILENO);
    int fd;

    if (argc == 1) {
        fd = open(".", O_RDONLY);
    } else {
        fd = open(argv[1], O_RDONLY);
    }
    check_err(fd, "open");

    struct ng_dirent dirent_buf[128];
    int entries = readdir(fd, dirent_buf, 128);
    check_err(entries, "readdir");

    for (int i = 0; i < entries; i++) {
        struct ng_dirent *entry = &dirent_buf[i];
        if (entry->filename[0] == '.') {
            // continue unless -a
        }
        if (ttyout) {
            printf("%s%c\n", entry->filename, ft_sigil(entry));
        } else {
            printf("%s\n", entry->filename);
        }
    }

    return EXIT_SUCCESS;
}
