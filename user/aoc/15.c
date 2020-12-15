#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int consume_char(char **buffer, char c) {
    if (c == **buffer) {
        (*buffer)++;
        return true;
    } else {
        return false;
    }
}

long consume_long(char **buffer) {
    char *nptr;
    assert(isdigit(**buffer));
    long l = strtol(*buffer, &nptr, 10);
    *buffer = nptr;
    return l;
}

long numbers[2021];

void interpret(char *buffer) {
    char *cursor_ = buffer;
    char **cursor = &cursor_;

    size_t x = 1;
    while (true) {
        numbers[x++] = consume_long(cursor);
        if (!consume_char(cursor, ',')) break;
    }

    for (ptrdiff_t i = x; i <= 2020; i++) {
        for (ptrdiff_t j = i-2; j > 0; j--) {
            if (numbers[j] == numbers[i-1]) {
                numbers[i] = i-1-j;
                break;
            }
        }
    }

    for (ptrdiff_t i = 1; i <= 2020; i++) {
        printf("[%5zi]: %10zi\n", i, numbers[i]);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "argument required\n");
        return 1;
    }

    interpret(argv[1]);
}
