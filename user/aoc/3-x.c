#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long tree_count(FILE *stream, long over, long down) {
    char buffer[128];
    long distance_over = 0;
    long row = -1;
    long trees = 0;
    while (fgets(buffer, 1024, stream)) {
        if (row++ % down != 0) continue;
        size_t n = distance_over % (strlen(buffer) - 1);
        if (buffer[n] == '#') {
            trees += 1;
        }
        distance_over += over;
    }
    fseek(stream, 0, SEEK_SET);
    return trees;
}

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

    for (int i=0; i<20; i++) {
        printf("tree_count(%i, 1) = %li\n", i, tree_count(file, i, 1));
    }
}
