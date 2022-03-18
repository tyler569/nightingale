#include <basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

int __ng_openat(int fd, const char *path, int flags, int mode);
int __ng_mkdirat(int fd, const char *path, int mode);
int __ng_pathname(int fd, char *buffer, size_t len);
int __ng_getdents(int fd, struct ng_dirent *, size_t);
int __ng_close(int fd);
int __ng_read(int fd, char *buffer, size_t);
int __ng_write(int fd, const char *buffer, size_t);
int __ng_fstat(int fd, struct stat *);
int __ng_linkat(int ofdat, const char *oldpath, int nfdst, const char *newpath);
int __ng_symlinkat(const char *topath, int fdat, const char *path);
int __ng_readlinkat(int fdat, const char *path, char *buffer, size_t len);
int __ng_mknodat(int fdat, const char *path, dev_t device, mode_t mode);
int __ng_pipe(int pipefds[static 2]);
int __ng_mountat(
    int atfd, const char *target, int type, int s_atfd, const char *source);
int __ng_dup(int fd);
int __ng_dup2(int fd, int newfd);

// dup, dup2, fchmod

_Noreturn void fail(const char *);
void check(int maybe_error, const char *message);
void check_nz(int maybe_error, const char *message);
void tree(const char *path);

int mkdirat_old(int atfd, const char *path, int mode)
{
    int err = __ng_mkdirat(atfd, path, mode);
    check(err, "mkdirat");
    int fd = __ng_openat(atfd, path, O_RDWR, 0);
    check(fd, "mkdirat open");
    return fd;
}

int main()
{
    int err;
    int a = mkdirat_old(AT_FDCWD, "/a", 0755);
    check(a, "mkdirat");
    printf("dir: %i\n", a);

    char buffer[100];
    char name[100] = { 0 };

    for (int i = 0; i < 2; i++) {
        snprintf(name, 100, "%i", i);
        a = mkdirat_old(a, name, 0755);
        check(a, "mkdirat a");

        for (int j = 0; j < 2; j++) {
            snprintf(name, 100, "file%i", j);
            int b = __ng_openat(a, name, O_CREAT | O_EXCL | O_WRONLY, 0644);
            check(b, "openat b");
            int n = __ng_write(b, "Hello World\n", 12);
            check(n, "write b");
            __ng_close(b);
        }
    }

    int c = mkdirat_old(AT_FDCWD, "/last", 0755);
    __ng_close(c);
    c = mkdirat_old(AT_FDCWD, "/a/0/last", 0755);
    __ng_close(c);

    struct stat statbuf;

    err = __ng_linkat(AT_FDCWD, "/bin/text_file", AT_FDCWD, "/a/hardlink");
    check(err, "linkat");

    err = __ng_symlinkat("/bin/text_file", AT_FDCWD, "/a/symlink");
    check(err, "symlinkat");

    err = __ng_symlinkat("/a/loop", AT_FDCWD, "/a/loop");
    check(err, "symlinkat");

    err = __ng_mknodat(AT_FDCWD, "/a/null", S_IFCHR | 0666, 0);
    check(err, "mknodat");

#define _FS_PROCFS 1
    err = mkdirat_old(AT_FDCWD, "/a/proc", 0755);
    check(err, "mkdirat proc");
    __ng_close(err);
    err = __ng_mountat(AT_FDCWD, "/a/proc", _FS_PROCFS, AT_FDCWD, "procfs");
    check(err, "mount 1");

    err = mkdirat_old(AT_FDCWD, "/a/0/1/proc", 0755);
    check(err, "mkdirat proc");
    __ng_close(err);
    err = __ng_mountat(AT_FDCWD, "/a/0/1/proc", _FS_PROCFS, AT_FDCWD, "procfs");
    check(err, "mount 2");

    tree("/a");

    c = __ng_openat(AT_FDCWD, "/a/0/file0", O_RDONLY, 0);
    __ng_read(c, buffer, 100);
    __ng_pathname(c, name, 100);
    __ng_fstat(c, &statbuf);

    printf("contents of \"%s\" (%i) are \"%s\"\n", name, c, buffer);
    printf("inode: %li, permissions: %#o\n", statbuf.st_ino, statbuf.st_mode);

    __ng_close(c);

    c = __ng_openat(AT_FDCWD, "/bin/text_file", O_RDONLY, 0);
    __ng_read(c, buffer, 100);
    __ng_pathname(c, name, 100);
    __ng_fstat(c, &statbuf);

    printf("contents of \"%s\" (%i) are \"%s\"\n", name, c, buffer);
    printf("inode: %li, permissions: %#o\n", statbuf.st_ino, statbuf.st_mode);

    c = __ng_openat(AT_FDCWD, "/a/hardlink", O_RDONLY, 0);
    check(c, "openat link");
    __ng_fstat(c, &statbuf);
    memset(name, 0, 100);
    __ng_pathname(c, name, 100);
    printf("%s has %i links\n", name, statbuf.st_nlink);
    __ng_close(c);

    err = __ng_readlinkat(AT_FDCWD, "/a/symlink", buffer, 100);
    check(err, "readlinkat");
    printf("readlink: \"/a/symlink\" -> \"%s\"\n", buffer);

    memset(buffer, 0, 100);
    memset(name, 0, 100);

    c = __ng_openat(AT_FDCWD, "/a/symlink", O_RDONLY, 0);
    check(c, "openat symlink");
    __ng_read(c, buffer, 100);
    __ng_pathname(c, name, 100);
    __ng_fstat(c, &statbuf);

    printf("contents of link target \"%s\" (%i) are \"%s\"\n", name, c, buffer);
    printf("inode: %li, permissions: %#o\n", statbuf.st_ino, statbuf.st_mode);
    __ng_close(c);

    c = __ng_openat(AT_FDCWD, "/a/null", O_RDWR, 0);
    check(c, "openat null");
    __ng_fstat(c, &statbuf);
    printf("null is type %i, dev %i, perm %#o\n", statbuf.st_mode >> 16,
        statbuf.st_rdev, statbuf.st_mode & 0xFFFF);

    err = __ng_write(c, "Hello", 5);
    check_nz(err, "write null");
    err = __ng_read(c, buffer, 100);
    check(err, "read null");
    if (err > 0)
        fail("read null read something");
    printf("/a/null behaves like /dev/null\n");

    c = __ng_dup(c);
    check(c, "dup2");
    err = __ng_read(c, buffer, 100);
    check(err, "read dup null");
    if (err > 0)
        fail("read dup null read something");

    c = __ng_dup2(c, 10);
    check(c, "dup2");
    err = __ng_read(10, buffer, 100);
    check(err, "read dup2 null");
    if (err > 0)
        fail("read dup2 null read something");

    c = __ng_openat(AT_FDCWD, "/a/loop", O_RDONLY, 0);
    perror("openning /a/loop");

    int pipefds[2];
    err = __ng_pipe(pipefds);
    check(err, "pipe");
    err = __ng_write(pipefds[1], "Hello", 5);
    if (err != 5)
        fail("write pipe");
    memset(buffer, 0, 100);
    err = __ng_read(pipefds[0], buffer, 100);
    if (err != 5)
        fail("read pipe");
    if (strcmp(buffer, "Hello") == 0)
        printf("pipe behaves like a pipe (at least in the trivial case)\n");

    c = __ng_openat(AT_FDCWD, "/a/proc/test", O_RDONLY, 0);
    check(c, "open proc");
    err = __ng_read(c, buffer, 100);
    check(err, "read proc");
    printf("/proc/test contains \"%.*s\"\n", err, buffer);
    __ng_close(c);

    c = __ng_openat(AT_FDCWD, "/a/proc/test", O_RDONLY, 0);
    check(c, "open proc");
    err = __ng_read(c, buffer, 100);
    check(err, "read proc");
    printf("/proc/test contains \"%.*s\"\n", err, buffer);
    __ng_close(c);
}

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
    struct ng_dirent dents[32] = { 0 };
    int number = __ng_getdents(fd, dents, ARRAY_LEN(dents));
    check(number, "getdents");
    for (int i = 0; i < number; i++) {
        print_levels(depth, levels);
        printf("%s", dents[i].name);
        switch (dents[i].type) {
        case FT_SYMLINK:
            __ng_readlinkat(fd, dents[i].name, buffer, 64);
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

        if (dents[i].type == FT_DIRECTORY) {
            int n = __ng_openat(fd, dents[i].name, O_RDONLY, 0644);
            check(n, "tree inner open");
            bool more = i != number - 1;
            tree_from(n, depth + 1, levels | (more << depth));
            __ng_close(n);
        }
    }
}

void tree(const char *path)
{
    int fd = __ng_openat(AT_FDCWD, path, O_RDONLY, 0644);
    check(fd, "tree open");
    printf("%s\n", path);
    tree_from(fd, 1, 0);
    __ng_close(fd);
}
