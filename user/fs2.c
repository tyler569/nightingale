#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

int __ng_openat2(int fd, const char *path, int flags, int mode);
int __ng_mkdirat2(int fd, const char *path, int mode);
int __ng_pathname2(int fd, char *buffer, size_t len);

_Noreturn void fail(const char *);

int main() {
    int fd = __ng_openat2(AT_FDCWD, "/", O_RDONLY, 0644);
    if (fd < 0)
        fail("openat");
    printf("fd: %i\n", fd);

    int a = __ng_mkdirat2(fd, "/a", 0644);
    if (a < 0)
        fail("mkdirat");
    printf("dir: %i\n", a);

    char buffer[100];

    for (int i = 0; i < 5; i++) {
        a = __ng_mkdirat2(a, (char[]){'a' + i, 0}, 0644);
        if (a < 0)
            fail("mkdirat");
        printf("dir: %i\n", a);

        int err = __ng_pathname2(a, buffer, 100);
        if (err < 0)
            fail("pathname");
        printf("path: %s\n", buffer);
    }
}

_Noreturn void fail(const char *message) {
    perror(message);
    exit(1);
}
