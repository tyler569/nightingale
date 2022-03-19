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

char ft_sigil(struct dirent *dirent)
{
    int type = dirent->d_type;
    int perm = dirent->d_mode;
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

int compare_dirent_ptrs(const void *a, const void *b)
{
    return strcmp(
        (*(struct dirent **)a)->d_name, (*(struct dirent **)b)->d_name);
}

struct dirent *dirent_ptrs[128];
char *dirent_buf[8192];

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

    int size = getdents(fd, (void *)dirent_buf, 8192);
    check_err(size, "readdir");

    size_t n_dirents = 0;
    for (size_t i = 0; i < size;) {
        struct dirent *d = PTR_ADD(dirent_buf, i);
        dirent_ptrs[n_dirents++] = d;
        i += d->d_reclen;
    }

    qsort(dirent_ptrs, n_dirents, sizeof(struct dirent *), compare_dirent_ptrs);

    if (!long_) {
        redirect_output_to((char *[]) { "/bin/column", NULL });
    }

    for (size_t i = 0; i < n_dirents; i++) {
        struct dirent *entry = dirent_ptrs[i];
        if (entry->d_name[0] == '.') {
            if (!all)
                continue;
        }

        if (classify) {
            printf("%s%c", entry->d_name, ft_sigil(entry));
        } else {
            printf("%s", entry->d_name);
        }

        if (long_ && entry->d_type == FT_SYMLINK) {
            char buffer[256] = { 0 };
            int err = readlinkat(fd, entry->d_name, buffer, 256);
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
