#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long tree_count(FILE *stream, long over, long down) {
    char buffer[1024];
    long distance_over = 0;
    long row = -1;
    long trees = 0;
    while (fgets(buffer, 1024, stream)) {
        row += 1;
        if (row % down != 0) continue;
        size_t n = distance_over % (strlen(buffer) - 1);
        if (buffer[n] == '#') {
            trees += 1;
            buffer[n] = 'X';
        } else {
            buffer[n] = 'O';
        }
        printf("%s", buffer);
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

    long trees1 = tree_count(file, 1, 1);
    long trees3 = tree_count(file, 3, 1);
    long trees5 = tree_count(file, 5, 1);
    long trees7 = tree_count(file, 7, 1);
    long trees2 = tree_count(file, 1, 2);

    printf("trees1: %li\n", trees1);
    printf("trees3: %li\n", trees3);
    printf("trees5: %li\n", trees5);
    printf("trees7: %li\n", trees7);
    printf("trees2: %li\n", trees2);

    printf("result: %li\n", trees1 * trees3 * trees5 * trees7 * trees2);
}
