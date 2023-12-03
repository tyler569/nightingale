#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char **argv)
{
    struct stat statbuf;
    const char *path = argv[1];

    if (argc < 2) {
        fprintf(stderr, "usage: stat filename\n");
        exit(1);
    }

    int err = stat(path, &statbuf);
    if (err != 0) {
        perror("stat");
        exit(1);
    }

    printf("dev:  %11i inode: %10li uid:  %10i gid: %10i\n", statbuf.st_dev,
        statbuf.st_ino, statbuf.st_uid, statbuf.st_gid);
    printf("mode: %5o       links: %10i size: %10li\n",
        statbuf.st_mode & 0xFFFF, statbuf.st_nlink, statbuf.st_size);

    int type = statbuf.st_mode >> 16;
    printf("type: %s (%c)\n", __filetype_names[type], __filetype_sigils[type]);

    char buffer[128];

#if 0
    if (type == FT_SYMLINK) {
        readlink(path, buffer);
        printf("links to: \"%s\"\n", buffer);
    }
#endif

    struct tm tm;
    gmtime_r(&statbuf.st_atime, &tm);
    strftime(buffer, 128, "%A %B %d, %Y at %r (%FT%T)", &tm);
    printf("atime: %s\n", buffer);
    gmtime_r(&statbuf.st_mtime, &tm);
    strftime(buffer, 128, "%A %B %d, %Y at %r (%FT%T)", &tm);
    printf("mtime: %s\n", buffer);
    gmtime_r(&statbuf.st_ctime, &tm);
    strftime(buffer, 128, "%A %B %d, %Y at %r (%FT%T)", &tm);
    printf("ctime: %s\n", buffer);
}
