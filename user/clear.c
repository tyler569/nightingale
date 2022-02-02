#include <stdio.h>
#include <stdlib.h>

#define MOVE_TO_TOP_LEFT "\x1B[H"
#define CLEAR_SCREEN "\x1B[2J"
#define CLEAR_SCROLLBACK "\x1B[3J"

int main() {
    printf(MOVE_TO_TOP_LEFT CLEAR_SCREEN CLEAR_SCROLLBACK);
    return EXIT_SUCCESS;
}
