#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

size_t line_count(FILE *f) {
    size_t nlines = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') nlines++;
    }
    fseek(f, 0, SEEK_SET);
    return nlines;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "argument bruh\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    size_t lines = line_count(f);

    long *numbers = malloc(lines * sizeof(long));

    size_t i = 0;
    char buffer[128] = {0};
    while (fgets(buffer, 128, f)) {
        assert(i < lines);
        long n = strtol(buffer, NULL, 10);
        numbers[i++] = n;
    }

    for (size_t x = 0; x < lines; x++) {
        for (size_t y = 0; y < lines; y++) {
            for (size_t z = 0; z < lines; z++) {
                if (numbers[x] + numbers[y] + numbers[z] == 2020) {
                    printf("%li %li %li\n", numbers[x], numbers[y], numbers[z]);
                    printf("answer: %li\n", numbers[x] * numbers[y] * numbers[z]);
                    return 0;
                }
            }
        }
    }
    fprintf(stderr, "no match found\n");
    return 0;
}
