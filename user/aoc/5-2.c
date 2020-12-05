#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long seat_id(const char *row) {
    int id = 0;
    for (int i=0; i<10; i++) {
        id <<= 1;
        if (row[i] == 'B' || row[i] == 'R') id |= 1;
    }
    // printf("%s: %i\n", row, id);
    return id;
}

void missing_seat_id(FILE *stream) {
    char seats[1000] = {0};
    char buffer[32];
    while(fgets(buffer, 32, stream)) {
        seats[seat_id(buffer)] = 1;
    }

    for (int i=1; i<999; i++) {
        if (seats[i] ^ seats[i+1]) {
            printf("seats[%i] = %hhi\n", i, seats[i]);
        }
    }
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

    missing_seat_id(file);
}
