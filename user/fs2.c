#include <basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

int __ng_openat2(int fd, const char *path, int flags, int mode);
int __ng_mkdirat2(int fd, const char *path, int mode);
int __ng_pathname2(int fd, char *buffer, size_t len);
int __ng_getdents2(int fd, struct ng_dirent *, size_t);
int __ng_close2(int fd);
int __ng_read2(int fd, char *buffer, size_t);
int __ng_write2(int fd, const char *buffer, size_t);
int __ng_fstat2(int fd, struct stat *);

_Noreturn void fail(const char *);
void tree(const char *path);

int main() {
    int a = __ng_mkdirat2(AT_FDCWD, "/a", 0644);
    if (a < 0)
        fail("mkdirat");
    printf("dir: %i\n", a);

    char buffer[100];
    char name[100] = {0};

    for (int i = 0; i < 2; i++) {
        snprintf(name, 100, "%i", i);
        a = __ng_mkdirat2(a, name, 0644);
        if (a < 0)
            fail("mkdirat a");

        for (int j = 0; j < 2; j++) {
            snprintf(name, 100, "file%i", j);
            int b = __ng_openat2(a, name, O_CREAT | O_EXCL | O_WRONLY, 0644);
            if (b < 0)
                fail("openat b");
            int n = __ng_write2(b, "Hello World\n", 12);
            if (n < 0)
                fail("write b");
            __ng_close2(b);
        }
    }

    int c = __ng_mkdirat2(AT_FDCWD, "/last", 0644);
    __ng_close2(c);
    c = __ng_mkdirat2(AT_FDCWD, "/a/0/last", 0644);
    __ng_close2(c);

    tree("/");

    struct stat statbuf;

    c = __ng_openat2(AT_FDCWD, "/a/0/file0", O_RDONLY, 0);
    __ng_read2(c, buffer, 100);
    __ng_pathname2(c, name, 100);
    __ng_fstat2(c, &statbuf);

    printf("contents of \"%s\" (%i) are \"%s\"\n", name, c, buffer);
    printf("inode: %li, permissions: %#o\n", statbuf.st_ino, statbuf.st_mode);

    __ng_close2(c);

    c = __ng_openat2(AT_FDCWD, "/bin/text_file", O_RDONLY, 0);
    __ng_read2(c, buffer, 100);
    __ng_pathname2(c, name, 100);
    __ng_fstat2(c, &statbuf);

    printf("contents of \"%s\" (%i) are \"%s\"\n", name, c, buffer);
    printf("inode: %li, permissions: %#o\n", statbuf.st_ino, statbuf.st_mode);
}

_Noreturn void fail(const char *message) {
    perror(message);
    exit(1);
}

void print_levels(int depth, int levels) {
    (void) levels;
    for (int i = 0; i < depth-1; i++) {
        if (levels & (1 << (i+1))) {
            printf("| ");
        } else {
            printf("  ");
        }
    }
    printf("+-");
}

void tree_from(int fd, int depth, int levels) {
    struct ng_dirent dents[32] = {0};
    int number = __ng_getdents2(fd, dents, ARRAY_LEN(dents));
    if (number < 0)
        fail("getdents");
    for (int i = 0; i < number; i++) {
        print_levels(depth, levels);
        printf("%s%s\n", dents[i].name, dents[i].type == FT_DIRECTORY ? "/" : "");

        if (dents[i].type == FT_DIRECTORY) {
            int n = __ng_openat2(fd, dents[i].name, O_RDONLY, 0644);
            if (n < 0)
                fail("tree inner open");
            bool more = i != number - 1;
            tree_from(n, depth + 1, levels | (more << depth));
            __ng_close2(n);
        }
    }
}

void tree(const char *path) {
    int fd = __ng_openat2(AT_FDCWD, path, O_RDONLY, 0644);
    if (fd < 0)
        fail("tree open");
    printf("%s\n", path);
    tree_from(fd, 1, 0);
    __ng_close2(fd);
}
