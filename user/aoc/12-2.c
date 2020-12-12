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

void rotate(enum turn dir, long amount, long *dn, long *de) {
    long magnitude = amount / 90;
    for (long i=0; i<magnitude; i++) {
        if (dir == LEFT) {
            long tmp = *de;
            *de = -*dn;
            *dn = tmp;
        } else {
            long tmp = *de;
            *de = *dn;
            *dn = -tmp;
        }
    }
}

long turtle_turt(FILE *stream) {
    long ns = 0;
    long ew = 0;

    long dn = 1;
    long de = 10;

    char buffer[512];
    while (fgets(buffer, 512, stream)) {
        //if (strlen(buffer) < 2) break;

        long arg = strtol(buffer + 1, NULL, 10);
        switch(buffer[0]) {
        case 'N': dn += arg; break;
        case 'S': dn -= arg; break;
        case 'E': de += arg; break;
        case 'W': de -= arg; break;

        case 'R': rotate(RIGHT, arg, &dn, &de); break;
        case 'L': rotate(LEFT, arg, &dn, &de); break;

        case 'F': {
            for (int i=0; i<arg; i++) {
                ns += dn;
                ew += de;
            }
        }
        }
        buffer[strlen(buffer)-1] = '\0';
        printf("\"%6s\": %5li %5li %3li, %3li\n", buffer, ns, ew, dn, de);
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
