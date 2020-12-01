#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s mode file (only supports octal modes)\n", argv[0]);
        return 1;
    }

    char *endptr;
    mode_t mode = strtol(argv[1], &endptr, 8);
    if (*endptr) {
        fprintf(stderr, "usage: %s mode file (only supports octal modes)\n", argv[0]);
        fprintf(stderr, "(%s did not parse as an octal number)\n", argv[1]);
        return 1;
    }

    int err = chmod(argv[2], mode);
    if (err) {
        perror("chmod");
        return 1;
    }
}
