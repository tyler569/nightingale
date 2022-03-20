#include <sys/stat.h>
#include <fcntl.h>

const char __filetype_sigils[] = {
    [FT_NORMAL] = ' ',
    [FT_CHAR_DEV] = '^',
    [FT_TTY] = '#',
    [FT_SOCKET] = ':',
    [FT_DIRECTORY] = '/',
    [FT_PIPE] = '|',
    [FT_PROC] = '%',
    [FT_SYMLINK] = '>',
    [FT_BLOCK] = '$',
};

const char *__filetype_names[] = {
    [FT_NORMAL] = "normal",
    [FT_CHAR_DEV] = "character device",
    [FT_TTY] = "tty",
    [FT_SOCKET] = "socket",
    [FT_DIRECTORY] = "directory",
    [FT_PIPE] = "pipe",
    [FT_PROC] = "procfile",
    [FT_SYMLINK] = "symbolic link",
    [FT_BLOCK] = "block device",
};

#ifndef __kernel__
int __ng_mkpipeat(int atfd, const char *path, mode_t mode);

int mkfifo(const char *path, mode_t mode)
{
    return __ng_mkpipeat(AT_FDCWD, path, mode);
}

int mkfifoat(int atfd, const char *path, mode_t mode)
{
    return __ng_mkpipeat(atfd, path, mode);
}
#endif
