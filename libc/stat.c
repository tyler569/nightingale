#include <sys/stat.h>
#include <fcntl.h>

int __ng_mkpipeat(int atfd, const char *path, mode_t mode);

int mkfifo(const char *path, mode_t mode)
{
    return __ng_mkpipeat(AT_FDCWD, path, mode);
}

int mkfifoat(int atfd, const char *path, mode_t mode)
{
    return __ng_mkpipeat(atfd, path, mode);
}
