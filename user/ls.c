#include <dirent.h>
#include <fcntl.h>
#include <nightingale.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void check_err(int code, const char *message) {
    if (code < 0) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

// clang-format off
const char filetype_sigils[] = {
    [FT_DIRECTORY] = '/',
    [FT_BUFFER]    = ' ',
    [FT_CHARDEV]   = '^',
    [FT_TTY]       = '#',
    [FT_SOCKET]    = ':',
    [FT_PIPE]      = '&',
    [FT_PROC]      = '%',
    [FT_PROC_THREAD] = '$',
};
// clang-format on

char ft_sigil(struct ng_dirent *dirent) {
    int type = dirent->type;
    int perm = dirent->permissions;
    if (type == FT_BUFFER && (perm & USR_EXEC)) {
        return '*';
    } else {
        return filetype_sigils[type];
    }
}

void help(const char *progname) {
    fprintf(stderr,
            "%s: usage ls [-alF]\n"
            "  -a     Show all files\n"
            "  -l     List files long form\n"
            "  -F     Show filetype sigils\n",
            progname);
}

int main(int argc, char **argv) {
    bool all = false;
    // Intentionally checking this before potentially redirecting output
    bool classify = isatty(STDOUT_FILENO);
    bool long_ = !classify;
    int fd, opt;

    while ((opt = getopt(argc, argv, "alF")) != -1) {
        switch (opt) {
        case 'a': all = true; break;
        case 'l': long_ = true; break;
        case 'F': classify = true; break;
        default: help(argv[0]); return 0;
        }
    }

    if (!long_) { redirect_output_to((char *[]){"/usr/bin/column", NULL}); }

    if (!argv[optind]) {
        fd = open(".", O_RDONLY);
    } else {
        // TODO: loop over remaining arguments and list them all
        fd = open(argv[optind], O_RDONLY);
    }
    check_err(fd, "open");

    struct ng_dirent dirent_buf[128];
    int entries = readdir(fd, dirent_buf, 128);
    check_err(entries, "readdir");

    for (int i = 0; i < entries; i++) {
        struct ng_dirent *entry = &dirent_buf[i];
        if (entry->filename[0] == '.') {
            if (!all) continue;
        }
        if (classify) {
            printf("%s%c\n", entry->filename, ft_sigil(entry));
        } else {
            printf("%s\n", entry->filename);
        }
    }

    return EXIT_SUCCESS;
}
