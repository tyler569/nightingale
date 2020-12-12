#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum direction {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3,
};

enum turn {
    LEFT,
    RIGHT,
};

enum direction turn(enum direction start, enum turn dir, long degrees) {
    enum direction out;
    if (dir == RIGHT) {
        out = (start + (degrees / 90)) % 4;
    } else {
        out = (start - (degrees / 90)) % 4;
    }

    while ((int)out < 0) out += 4;
    return out;
}

long turtle_turt(FILE *stream) {
    long ns = 0;
    long ew = 0;
    enum direction dir = EAST;

    char buffer[512];
    while (fgets(buffer, 512, stream)) {
        //if (strlen(buffer) < 2) break;

        long arg = strtol(buffer + 1, NULL, 10);
        switch(buffer[0]) {
        case 'N': ns += arg; break;
        case 'S': ns -= arg; break;
        case 'E': ew += arg; break;
        case 'W': ew -= arg; break;

        case 'R': dir = turn(dir, RIGHT, arg); break;
        case 'L': dir = turn(dir, LEFT, arg); break;

        case 'F': {
            switch (dir) {
            case NORTH: ns += arg; break;
            case SOUTH: ns -= arg; break;
            case EAST : ew += arg; break;
            case WEST : ew -= arg; break;
            }
        }
        }

        buffer[strlen(buffer)-1] = '\0';
        printf("\"%5s\": %3li %3li %i\n", buffer, ns, ew, dir);
    }

    return max(ns, -ns) + max(ew, -ew);
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

    long answer = turtle_turt(file);
    printf("answer: %li\n", answer);
}
