#include <basic.h>
#include <sys/types.h>

static void swap(void *base, size_t _a, size_t _b, size_t size) {
    char *a = PTR_ADD(base, _a * size);
    char *b = PTR_ADD(base, _b * size);
    char c;

    for (size_t i = 0; i < size; i++) {
        c = a[i];
        a[i] = b[i];
        b[i] = c;
    }
}

static size_t partition(
        void *base,
        size_t nmemb,
        size_t size,
        int (*compar)(const void *, const void *)
) {
    size_t pivot = nmemb - 1;
    void *pivot_e = PTR_ADD(base, pivot * size);
    size_t i = -1;

    for (size_t j = 0; j < nmemb - 1; j++) {
        if (compar(PTR_ADD(base, size * j), pivot_e) <= 0) {
            i += 1;
            swap(base, i, j, size);
        }
    }
    i += 1;
    swap(base, pivot, i, size);
    return i;
}

void qsort(
        void *base,
        size_t nmemb,
        size_t size,
        int (*compar)(const void *, const void *)
) {
    if ((ssize_t)nmemb > 1) {
        size_t pi = partition(base, nmemb, size, compar);
        qsort(base, pi, size, compar);
        qsort(PTR_ADD(base, size * (pi + 1)), nmemb - pi - 1, size, compar);
    }
}
