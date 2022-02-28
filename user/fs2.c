#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

int __ng_openat2(int fd, const char *path, int mode);

int main() {
    int fd = __ng_openat2(AT_FDCWD, "/", 0644);
    if (fd < 0) {
        perror("openat");
    }
    printf("fd: %i\n", fd);
}
