#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nightingale.h>
#include <signal.h>
#include <unistd.h>

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

    int c;
    volatile int *x = NULL;
    while ((c = getopt(argc, argv, "asgAS")) != -1) {
        switch (c) {
        case 'a':
            assert(0);
        case 's':
            return *x;
        case 'A':
            fault(ASSERT);
        case 'S':
            fault(NULL_DEREF);
        case '?':
            usage();
            exit(1);
        }
    }

    return EXIT_SUCCESS;
}
