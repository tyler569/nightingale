
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ttyctl.h>

int isatty(int fd) {
        return ttyctl(fd, TTY_ISTTY, 0);
}

int execvp(const char *path, char *const argv[]) {
        return execve(path, argv, NULL);
}

off_t lseek(int fd, off_t offset, int whence) {
        return seek(fd, offset, whence);
}

int sleep(int seconds) {
        return sleepms(seconds * 1000);
}

