#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[1024];

enum {
    byr = (1 << 0),
    iyr = (1 << 1),
    eyr = (1 << 2),
    hgt = (1 << 3),
    hcl = (1 << 4),
    ecl = (1 << 5),
    pid = (1 << 6),
    cid = (1 << 7),
};

int is_valid(unsigned has) {
    return (has | cid) == 0xFF;
}

void print_problems(unsigned has) {
    if (!(has & byr)) printf("  - bad birth year\n");
    if (!(has & iyr)) printf("  - bad issuance\n");
    if (!(has & eyr)) printf("  - bad exiry\n");
    if (!(has & hgt)) printf("  - bad height\n");
    if (!(has & hcl)) printf("  - bad hair color\n");
    if (!(has & ecl)) printf("  - bad eye color\n");
    if (!(has & pid)) printf("  - bad passport ID\n");
    if (!(has & cid)) printf("  - bad country ID\n");
}

int validate_year(const char *string, int min, int max) {
    char *after;
    long year = strtol(string, &after, 10);
    if (year < min || year > max) return 0;
    if (!isspace(*after)) return 0;
    return 1;
}

int validate_byr(const char *string) {
    return validate_year(string, 1920, 2002);
}
int validate_iyr(const char *string) {
    return validate_year(string, 2010, 2020);
}
int validate_eyr(const char *string) {
    return validate_year(string, 2020, 2030);
}

int validate_hgt(const char *string) {
    char *after;
    long value = strtol(string, &after, 10);
    if (!isspace(after[2])) return 0;
    if (strncmp(after, "cm", 2) == 0) {
        return (value >= 150 && value <= 193);
    } else if (strncmp(after, "in", 2) == 0) {
        return (value >= 59 && value <= 76);
    } else {
        return 0;
    }
}

int validate_hcl(const char *string) {
    if (string[0] != '#') return 0;

    for (int i=1; i<7; i++) {
        if (!isxdigit(string[i])) return 0;
    }
    return 1;
}

int validate_ecl(const char *string) {
    // if (!isspace(string[4])) return 0;
    return
        strncmp(string, "amb", 3) == 0 ||
        strncmp(string, "blu", 3) == 0 ||
        strncmp(string, "brn", 3) == 0 ||
        strncmp(string, "gry", 3) == 0 ||
        strncmp(string, "grn", 3) == 0 ||
        strncmp(string, "hzl", 3) == 0 ||
        strncmp(string, "oth", 3) == 0;
}

int validate_pid(const char *string) {
    for (int i=0; i<9; i++) {
        if (!isdigit(string[i])) return 0;
    }
    if (!isspace(string[9])) return 0;
    return 1;
}

#define validate(N) if (strncmp(cursor, #N, 3) == 0 &&validate_ ## N(&cursor[4])) has |= N;

int check_passport(FILE *stream) {
    // read one passport and output its validityS
    unsigned has = 0;
    const char *cursor;
    while (fgets(buffer, 1024, stream)) {
        cursor = buffer;
        if (strlen(buffer) < 2) break;
        do {
            validate(byr);
            validate(iyr);
            validate(eyr);
            validate(hgt);
            validate(hcl);
            validate(ecl);
            validate(pid);
            if (strncmp(cursor, "cid", 3) == 0) has |= cid;

            cursor = strchr(cursor, ' ');
        } while (cursor++);
        // printf("%s", buffer);
    }
    // printf("\n%s\n\n", is_valid(has) ? "VALID" : "INVALID"); 
    // print_problems(has);
    // printf("\n\n");
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
