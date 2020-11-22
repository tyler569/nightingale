#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char rot13(char c) {
    if ((c >= 'a' && c < 'n') || (c >= 'A' && c < 'N')) { return c + 13; }
    if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) { return c - 13; }
    return c;
}

int main(int argc, char **argv) {
    char buf[128] = {0};

    int fd = STDIN_FILENO;
    int count;

    while ((count = read(fd, buf, 128)) > 0) {
        for (int i = 0; i < count; i++) {
            if (isalpha(buf[i])) buf[i] = rot13(buf[i]);
        }
        write(STDOUT_FILENO, buf, count);
    }

    if (count < 0) {
        perror("read()");
        return EXIT_FAILURE;
    }

    if (fd > 2) {
        int err = close(fd);
        if (err) perror("close()");
    }
    return EXIT_SUCCESS;
}
