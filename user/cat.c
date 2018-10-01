
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fnctl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("provide a file name (for now...)\n");
        return 1;
    }

    int fd = open(argv[1], 0);
    if (fd < 0) {
        perror("open()");
        return 2;
    }

    char buf[129] = {0};
    
    int count;
    int total_count = 0;

    while ((count = read(fd, buf, 128)) > 0) {
        buf[count] = '\0';

        printf("%s", buf);

        total_count += count;
    }

    if (count < 0) {
        perror("read()");
        return 3;
    }

    // printf("printed %i\n", total_count);

    return 0;
}

