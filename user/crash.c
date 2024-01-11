#include <assert.h>
#include <nightingale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage()
{
    printf("usage: crash [option]\n");
    printf("  -g: crash in signal handler\n");
    printf("  -s: usermode null deref\n");
    printf("  -S: kernel null deref\n");
    printf("  -a: usermode assertion\n");
    printf("  -A: kernel mode assertion\n");
    printf("  -j: usermode jump to 0\n");
    printf("  -J: kernel jump to 0\n");

    exit(0);
}

void segv_handler(int signal)
{
    printf("recieved SIGSEGV\n");
    exit(1);
}

volatile int *volatile ptr;
void (*volatile fun)();

void int_handler(int signal)
{
    printf("recieved SIGINT\n");
    *ptr = 1;
    exit(1);
}

int main(int argc, char **argv)
{
    signal(SIGSEGV, segv_handler);

    if (argc < 2)
        usage();

    int c;
    while ((c = getopt(argc, argv, "aAsSjJg")) != -1) {
        switch (c) {
        case 'a':
            assert(0);
        case 'A':
            fault(ASSERT);
            break;
        case 's':
            *ptr = 1;
            break;
        case 'S':
            fault(NULL_DEREF);
            break;
        case 'j':
            fun();
            break;
        case 'J':
            fault(NULL_JUMP);
            break;
        case 'g':
            signal(SIGINT, int_handler);
            raise(SIGINT);
            break;
        case '?':
            usage();
            exit(1);
        }
    }

    return EXIT_SUCCESS;
}
