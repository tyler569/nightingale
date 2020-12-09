#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t line_count(FILE *f) {
    size_t nlines = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') nlines++;
    }
    fseek(f, 0, SEEK_SET);
    return nlines;
}


long *numbers(FILE *stream, size_t count) {
    long *nums = malloc(sizeof(long) * count);
    char buffer[32];
    size_t i = 0;
    while (fgets(buffer, 32, stream)) {
        nums[i++] = strtol(buffer, NULL, 10);
    }
    return nums;
}

#define ANSWER 1124361034

long long_sum(long *numbers, size_t count) {
    for (size_t i = 0; i < count; i++) {
        long acc = 0;
        size_t x = 0;
        while (acc < ANSWER) {
            acc += numbers[i+x++];
        }
        if (acc == ANSWER) {
            long mn = ANSWER+1, mx = -99999999;

            for (size_t z = 0; z <= x; z++) {
                mn = min(numbers[i+z], mn);
                mx = max(numbers[i+z], mx);
            }

            printf("answer: %li\n", mn + mx);
        }
    }

    return 0;
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

    size_t len = line_count(file);

    long *nums = numbers(file, len);
    long_sum(nums, len);
}
