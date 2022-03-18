#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MOVE_TO_TOP_LEFT "\x1B[H"
#define CLEAR_SCREEN "\x1B[2J"
#define CLEAR_SCROLLBACK "\x1B[3J"

_Noreturn void help(const char *name)
{
    fprintf(stderr,
        "%s: clear the screen\n"
        "    -x   Don't clear scrollback\n",
        name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    char c = 0;
    int clear_scrollback = 1;

    while ((c = getopt(argc, argv, "x")) != -1) {
        switch (c) {
        case 'x':
            clear_scrollback = 0;
            break;
        default:
            help(argv[0]);
        }
    }

    printf(MOVE_TO_TOP_LEFT CLEAR_SCREEN);
    if (clear_scrollback) {
        printf(CLEAR_SCROLLBACK);
    }
    return EXIT_SUCCESS;
}
