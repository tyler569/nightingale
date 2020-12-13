#include <stdio.h>

typedef int qsort_compare(const void *, const void *);

static void swap(size_t size, char *a, char *b) {
    for (size_t i = 0; i < size; i++) {
        char tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

static size_t partition(char *base, qsort_compare *cmp, size_t size, long lo, long hi) {
    void *pivot = &base[hi*size];
    long i = lo;
    for (long j = lo; j <= hi; j++) {
        if (cmp(&base[j*size], &pivot) > 0) {
            swap(size, &base[i*size], &base[j*size]);
            i++;
        }
    }
    swap(size, &base[i*size], &base[hi*size]);

    return i;
}

static void qsort_internal(void *base, qsort_compare *cmp, size_t size, long lo, long hi) {
    // fprintf(stderr, "qs(%p, %li, %li)", (void *)base, lo, hi);
    if (lo < hi) {
        long p = partition(base, cmp, size, lo, hi);
        qsort_internal(base, cmp, size, lo, p - 1);
        qsort_internal(base, cmp, size, p + 1, hi);
    }
}

void mqsort(void *base, size_t nmemb, size_t size, qsort_compare *cmp) {
    qsort_internal(base, cmp, size, 0, nmemb-1);
}

int cmp_int(const void *a, const void *b) {
    return *(int *)b - *(int *)a;
}

#define asize(A) (sizeof(A) / sizeof(*A))

int main() {
    int foo[] = {4, 12, 3, 100, 8, 99, 6, 4, 5, 3, 1};

    mqsort(foo, asize(foo), sizeof(*foo), cmp_int);

    for (int i=0; i<asize(foo); i++)
        printf("%i\n", foo[i]);
}
