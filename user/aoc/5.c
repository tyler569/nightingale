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

long max_seat_id(FILE *stream) {
    char buffer[32];
    long max_id = 0;
    while(fgets(buffer, 32, stream)) {
        max_id = max(max_id, seat_id(buffer));
    }
    return max_id;
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

    printf("max id: %li\n", max_seat_id(file));
}
