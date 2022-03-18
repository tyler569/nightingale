#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nightingale.h>
#include <signal.h>

void usage()
{
    printf("usage: crash [option]\n");
    printf("  -s: null deref (userland)\n");
    printf("  -S: null deref (syscall)\n");
    printf("  -a: usermode assertion\n");
    printf("  -A: kernel mode assertion\n");

    exit(0);
}

void segv_handler(int signal)
{
    printf("recieved SIGSEGV\n");
    exit(1);
}

int main(int argc, char **argv)
{
    signal(SIGSEGV, segv_handler);

    if (argc < 2)
        usage();

    if (strcmp(argv[1], "-s") == 0) {
        volatile int *x = 0;
        return *x;
    } else if (strcmp(argv[1], "-a") == 0) {
        assert(0);
    } else if (strcmp(argv[1], "-A") == 0) {
        fault(ASSERT);
    } else if (strcmp(argv[1], "-S") == 0) {
        fault(NULL_DEREF);
    } else {
        usage();
    }

    return EXIT_SUCCESS;
}
