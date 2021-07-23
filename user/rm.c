#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int err;
    if (argc > 1) {
        err = unlink(argv[1]);
        if (err) perror("unlink");
    } else {
        fprintf(stderr, "%s: no argument provided", argv[0]);
    }
    return 0;
}
