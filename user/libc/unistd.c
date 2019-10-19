
#include <unistd.h>

int isatty(int fd) {
        // does not account for piped stdins.
        return fd < 3;
}

int execvp(const char *path, char *const argv[]) {
        return execve(path, argv, NULL);
}

