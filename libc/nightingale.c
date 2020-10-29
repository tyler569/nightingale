#include <basic.h>
#include <dirent.h>

ssize_t getdirents(int fd, struct ng_dirent *buf, size_t count) {
        return readdir(fd, buf, count);
}
