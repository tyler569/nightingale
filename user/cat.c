#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void copy(FILE *out, FILE *in) {
#define BUFSZ 4096
    int count;
    char buf[BUFSZ] = {0};

    while ((count = fread(buf, 1, BUFSZ, in)) > 0) {
        fwrite(buf, 1, count, out);
    }

    if (count < 0) perror("read()");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        copy(stdout, stdin);
        return EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; i++) {
        FILE *f;
        if (strcmp(argv[i], "-") == 0) {
            f = stdin;
        } else {
            f = fopen(argv[i], "r");
            if (!f) {
                perror("open()");
                return EXIT_FAILURE;
            }
        }

        copy(stdout, f);

        if (fileno(f) > 2) {
            int err = fclose(f);
            if (err) perror("close()");
        }
    }
    return EXIT_SUCCESS;
}
