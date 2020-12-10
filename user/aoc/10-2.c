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


long *adapter_numbers(FILE *stream, size_t count) {
    long *nums = malloc(sizeof(long) * count);
    nums[0] = 0;
    char buffer[32];
    size_t i = 1;
    long mx = 0, num;
    while (fgets(buffer, 32, stream)) {
        num = strtol(buffer, NULL, 10);
        mx = max(mx, num);

        nums[i++] = num;
    }
    nums[i] = mx + 3;
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


// long possibilities(long *numbers, size_t len) {
//     size_t ways = 1;
//     for (size_t i = 0; i < len; i++) {
//         // printf("n: %5li ", numbers[i]);
//         size_t j = i + 1;
//         size_t iways = 0;
//         while (j < len && numbers[j] <= numbers[i] + 3) {
//             iways += 1;
//             j++;
//         }
//         // printf("iways: %3zu\n", iways);
//         if (iways == 0) iways = 1;
//         ways *= iways;
//     }
//     return ways;
// }

long memo[1000];

long possibility_tree(long *numbers, size_t ix, size_t len) {
    size_t ways = 0;
    if (memo[ix] != -1) return memo[ix];

    long *xxx = numbers + ix;
    long this = xxx[0];
    for (int i = 1; i < len; i++) {
        if (xxx[i] > this + 3) break;
        if (i > 1) ways += 1;
        ways += possibility_tree(numbers, ix + i, len - i);
    }
    memo[ix] = ways;
    return ways;
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

    size_t len = line_count(file) + 1;

    long *nums = adapter_numbers(file, len);
    mqsort(nums, len);

    for (int i = 0; i < 1000; i++) memo[i] = -1;

    printf("%li\n", possibility_tree(nums, 0, len) + 1);
}
