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

#define PREAMBLE 25

long not_sum(long *numbers, size_t count) {
    for (size_t i = PREAMBLE; i < count; i++) {
        for (size_t x = 1; x <= PREAMBLE; x++) {
            for (size_t y = x + 1; y <= PREAMBLE; y++) {
                if (numbers[i] == numbers[i-x] + numbers[i-y]) goto next;
            }
        }
        return numbers[i];
next:;
    }
    assert(0 && "NO ANSWER");
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
    long answer = not_sum(nums, len);
    printf("answer: %li\n", answer);
}
