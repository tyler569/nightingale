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
    fgets(buffer, 1024, file);
    long time = strtol(buffer, NULL, 10);

    long mintime = 100000;
    long bestbus = 0;

    fgets(buffer, 1024, file);
    while (after && *after && *after != '\n') {
        if (*after == 'x') { after += 2; continue; }

        long id = strtol(after, &after, 10);
        long time_to_wait = id - (time % id);
        if (time_to_wait < mintime) {
            mintime = time_to_wait;
            bestbus = id;
        }

        if (*after == ',') after++;
    }

    printf("answer: %li\n", mintime * bestbus);
}
