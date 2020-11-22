#include <basic.h>
#include <dirent.h>

ssize_t readdir(int fd, struct ng_dirent *buf, size_t count);

ssize_t getdirents(int fd, struct ng_dirent *buf, size_t count) {
    return readdir(fd, buf, count);
}
