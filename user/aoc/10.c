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

void swap(long *a, long *b) {
    long tmp = *a;
    *a = *b;
    *b = tmp;
}

size_t partition(long *base, size_t lo, size_t hi) {
    long pivot = base[hi];
    size_t i = lo;
    for (size_t j = lo; j <= hi; j++) {
        if (base[j] < pivot) {
            swap(&base[i], &base[j]);
            i++;
        }
    }
    swap(&base[i], &base[hi]);

    return i;
}

void qsort_internal(long *base, size_t lo, size_t hi) {
    // printf("qs(%p, %zu, %zu)\n", (void *)base, lo, hi);
    if (lo < hi) {
        size_t p = partition(base, lo, hi);
        qsort_internal(base, lo, p - 1);
        qsort_internal(base, p + 1, hi);
    }
}

void mqsort(long *base, size_t nmemb) {
    qsort_internal(base, 0, nmemb - 1);
}


long differences(long *numbers, size_t len) {
    int j1 = 0, j3 = 0;

    // wall adapter is 0
    switch (numbers[0]) {
    case 1: j1++; break;
    case 3: j3++; break;
    }

    for (size_t i = 1; i < len; i++) {
        long diff = numbers[i] - numbers[i-1];
        switch (diff) {
        case 1: j1++; break;
        case 3: j3++; break;
        }
    }

    j3 += 1; // my device is always highest + 3;

    printf("j1: %i\n", j1);
    printf("j3: %i\n", j3);

    return j1 * j3;
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

    size_t len = line_count(file) - 1;

    long *nums = numbers(file, len);
    mqsort(nums, len);

    printf("%li\n", differences(nums, len));
}
