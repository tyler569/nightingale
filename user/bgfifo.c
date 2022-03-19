#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    if (fork() != 0)
        return 0;
    setpgid(0, 0);
    // unlink("/bin/fifo");
    int err = mkfifo("/bin/fifo", 0644);
    if (err) {
        perror("mkfifo");
        exit(1);
    }

    int fd = open("/bin/fifo", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char buffer[256];
    int len;

    while ((len = read(fd, buffer, 256))) {
        if (len < 0) {
            perror("read");
            exit(1);
        }
        printf("read: \"%.*s\"\n", len, buffer);
    }

    printf("done\n");
}
