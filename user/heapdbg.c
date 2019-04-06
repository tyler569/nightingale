
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <nightingale.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You must specify detail, summary, or both");
        return 1;
    }

    if (strcmp(argv[1], "summary") == 0) {
        errno = 0;
        heapdbg(HEAPDBG_SUMMARY);
        return errno;
    }
    if (strcmp(argv[1], "detail") == 0) {
        errno = 0;
        heapdbg(HEAPDBG_DETAIL);
        return errno;
    }
    if (strcmp(argv[1], "both") == 0) {
        errno = 0;
        heapdbg(HEAPDBG_DETAIL);
        heapdbg(HEAPDBG_SUMMARY);
        return errno;
    }

    printf("invalid argument: '%s'\n", argv[1]);
    return 1;
}

