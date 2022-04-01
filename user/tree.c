#include <basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

_Noreturn void fail(const char *message)
{
    perror(message);
    exit(1);
}

void check(int maybe_error, const char *message)
{
    if (maybe_error < 0 && errno != EEXIST)
        fail(message);
    errno = 0;
}

void check_nz(int maybe_error, const char *message)
{
    if (maybe_error <= 0 && errno != EEXIST)
        fail(message);
    errno = 0;
}

void print_levels(int depth, int levels)
{
    (void)levels;
    for (int i = 0; i < depth - 1; i++) {
        if (levels & (1 << (i + 1))) {
            printf("| ");
        } else {
            printf("  ");
        }
    }
    printf("+-");
}

void tree_from(int fd, int depth, int levels)
{
    char buffer[65] = { 0 };
    char dents[2048];
    int size = getdents(fd, (void *)dents, sizeof(dents));
    check(size, "getdents");
    for (int i = 0; i < size;) {
        struct dirent *dent = PTR_ADD(dents, i);
        print_levels(depth, levels);
        printf("%s", dent->d_name);
        switch (dent->d_type) {
        case FT_SYMLINK:
            readlinkat(fd, dent->d_name, buffer, 64);
            printf(" -> \x1b[31m%s\x1b[0m", buffer);
            break;
        case FT_DIRECTORY:
            printf("/");
            break;
        case FT_CHAR_DEV:
            printf("$");
            break;
        default:;
        };
        printf("\n");

        if (dent->d_type == FT_DIRECTORY) {
            int n = openat(fd, dent->d_name, O_RDONLY, 0644);
            check(n, "tree inner open");
            bool more = i + dent->d_reclen != size;
            tree_from(n, depth + 1, levels | (more << depth));
            close(n);
        }

        i += dent->d_reclen;
    }
}

void tree(const char *path)
{
    int fd = openat(AT_FDCWD, path, O_RDONLY, 0644);
    check(fd, "tree open");
    printf("%s\n", path);
    tree_from(fd, 1, 0);
    close(fd);
}

int main(int argc, char **argv)
{
    if (argc == 1)
        tree(".");
    else
        tree(argv[1]);
}
