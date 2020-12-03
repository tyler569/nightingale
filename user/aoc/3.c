#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tree_count(FILE *stream, int over, int down) {
    char buffer[1024];
    int distance_over = 0;
    int trees = 0;
    while (fgets(buffer, 1024, stream)) {
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

    int trees = tree_count(file, 3, 1);
    printf("trees: %i\n", trees);
}
