#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

char consume_char(char **buffer) {
    char c = **buffer;
    *buffer += 1;
    return c;
}

void eat_char(char **buffer, char c) {
    assert(c == **buffer);
    *buffer += 1;
}

long consume_long(char **buffer) {
    char *nptr;
    assert(isdigit(**buffer));
    long l = strtol(*buffer, &nptr, 10);
    *buffer = nptr;
    return l;
}

int count_char(char *string, char c) {
    int count = 0;
    for (int i = 0; string[i]; i++) {
        if (string[i] == c) count++;
    }
    return count;
}

int test_password_2(char *dbline) {
    char *cursor = &dbline[0];

    long min, max;
    char letter;
    char *password;
    
    min = consume_long(&cursor);
    eat_char(&cursor, '-');
    max = consume_long(&cursor);
    eat_char(&cursor, ' ');
    letter = consume_char(&cursor);
    eat_char(&cursor, ':');
    eat_char(&cursor, ' ');

    password = cursor;

    printf("consider %li %li %c %s", min, max, letter, password);

    return (password[min-1] == letter) ^ (password[max-1] == letter);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "argument required\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char *c;
    char buffer[1024];

    int npasswords = 0;
    int nvalid = 0;

    while ((c = fgets(buffer, 1024, f))) {
        npasswords += 1;
        nvalid += test_password_2(buffer);
    }

    printf("passwords: %i, valid: %i\n", npasswords, nvalid);
}
