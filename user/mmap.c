#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("text_file", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    void *addr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("%s", (char *)addr);
    return 0;
}
