#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char rot(char c, int n) {
    if (c >= 'a' && c < 'z') return ((c - 'a') + n) % 26 + 'a';
    if (c >= 'A' && c < 'Z') return ((c - 'A') + n) % 26 + 'A';
    return c;
}

int main(int argc, char **argv) {
    int c;
    int n = 13;
    while ((c = getopt(argc, argv, "n:")) != -1) {
        switch (c) {
        case 'n': n = strtol(optarg, NULL, 10); break;
        case '?': fprintf(stderr, "usage: rot13 [-n rot]\n"); return 0;
        }
    }
    char buf[128] = {0};

    int fd = STDIN_FILENO;
    int count;

    while ((count = read(fd, buf, 128)) > 0) {
        for (int i = 0; i < count; i++) {
            if (isalpha(buf[i])) buf[i] = rot(buf[i], n);
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
