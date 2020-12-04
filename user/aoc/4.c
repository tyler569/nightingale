#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[1024];

enum {
    BYR = (1 << 0),
    IYR = (1 << 1),
    EYR = (1 << 2),
    HGT = (1 << 3),
    HCL = (1 << 4),
    ECL = (1 << 5),
    PID = (1 << 6),
    CID = (1 << 7),
};

int is_valid(unsigned has) {
    return (has | CID) == 0xFF;
}

int check_passport(FILE *stream) {
    // read one passport and output its validityS
    unsigned has = 0;
    const char *cursor;
    while (fgets(buffer, 1024, stream)) {
        cursor = buffer;
        if (strlen(buffer) < 2) break;
        do {
            if (strncmp(cursor, "byr", 3) == 0) has |= BYR;
            if (strncmp(cursor, "iyr", 3) == 0) has |= IYR;
            if (strncmp(cursor, "eyr", 3) == 0) has |= EYR;
            if (strncmp(cursor, "hgt", 3) == 0) has |= HGT;
            if (strncmp(cursor, "hcl", 3) == 0) has |= HCL;
            if (strncmp(cursor, "ecl", 3) == 0) has |= ECL;
            if (strncmp(cursor, "pid", 3) == 0) has |= PID;
            if (strncmp(cursor, "cid", 3) == 0) has |= CID;

            cursor = strchr(cursor, ' ');
        } while (cursor++);
    }
    return is_valid(has);
}

long check_passport_batch(FILE *stream) {
    long valid_count = 0;
    while (!feof(stream)) valid_count += check_passport(stream);
    return valid_count;
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


    printf("valid passports: %li\n", check_passport_batch(file));
}
