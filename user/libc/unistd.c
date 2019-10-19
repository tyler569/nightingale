
#include <unistd.h>
#include <sys/types.h>

int isatty(int fd) {
        // does not account for piped stdins.
        return fd < 3;
}

int execvp(const char *path, char *const argv[]) {
        return execve(path, argv, NULL);
}

off_t lseek(int fd, off_t offset, int whence) {
        return seek(fd, offset, whence);
}

