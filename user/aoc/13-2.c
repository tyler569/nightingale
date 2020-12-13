#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "argument required");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    char buffer[1024];
    char *after = buffer;
    fgets(buffer, 1024, file); // discard

    long mintime = 100000;
    long bestbus = 0;

    fgets(buffer, 1024, file);
    long n = 0;
    long minv = 0;
    long p = 1;
    while (after && *after && *after != '\n') {
        if (*after == 'x') { n++; after += 2; continue; }

        long id = strtol(after, &after, 10);
        if (*after == ',') after++;

        while ((minv + n) % id != 0) {
            minv += p;
        }
        p *= id;

        printf("%li, %li\n", minv, p);

        n++;
    }

    printf("answer: %li\n", minv);
}
