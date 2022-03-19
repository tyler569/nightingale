#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <nightingale.h>
#include <unistd.h>

void check_err(int code, const char *message)
{
    if (code < 0) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

char ft_sigil(struct ng_dirent *dirent)
{
    int type = dirent->type;
    int perm = dirent->mode;
    if (type == FT_NORMAL && (perm & USR_EXEC)) {
        return '*';
    } else {
        return __filetype_sigils[type];
    }
}

void help(const char *progname)
{
    fprintf(stderr,
        "%s: usage ls [-alF]\n"
        "  -a     Show all files\n"
        "  -l     List files long form\n"
        "  -F     Show filetype sigils\n",
        progname);
}

int compare_dirents(const void *a, const void *b)
{
    return strcmp(((struct ng_dirent *)a)->name, ((struct ng_dirent *)b)->name);
}

int main(int argc, char **argv)
{
    bool all = false;
    // Intentionally checking this before potentially redirecting output
    bool classify = isatty(STDOUT_FILENO);
    bool long_ = !classify;
    int fd, opt;

    while ((opt = getopt(argc, argv, "alF")) != -1) {
        switch (opt) {
        case 'a':
            all = true;
            break;
        case 'l':
            long_ = true;
            break;
        case 'F':
            classify = true;
            break;
        default:
            help(argv[0]);
            return 0;
        }
    }

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

    qsort(dirent_buf, entries, sizeof(struct ng_dirent), compare_dirents);

    if (!long_) {
        redirect_output_to((char *[]) { "/bin/column", NULL });
    }

    for (int i = 0; i < entries; i++) {
        struct ng_dirent *entry = &dirent_buf[i];
        if (entry->name[0] == '.') {
            if (!all)
                continue;
        }

        if (classify) {
            printf("%s%c", entry->name, ft_sigil(entry));
        } else {
            printf("%s", entry->name);
        }

        if (long_ && entry->type == FT_SYMLINK) {
            char buffer[256] = { 0 };
            int err = readlinkat(fd, entry->name, buffer, 256);
            if (err < 0) {
                perror("readlinkat");
                printf("\n");
                continue;
            }
            printf(" -> \x1b[31m%s\x1b[m", buffer);
        }

        printf("\n");
    }

    return EXIT_SUCCESS;
}
