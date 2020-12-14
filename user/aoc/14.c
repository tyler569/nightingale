#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char consume_char(char **buffer) {
    char c = **buffer;
    *buffer += 1;
    return c;
}

void eat_char(char **buffer, char c) {
    assert(c == **buffer);
    *buffer += 1;
}

void eat_string(char **buffer, char *s) {
    for (; *s; s++) eat_char(buffer, *s);
}

long consume_long(char **buffer) {
    char *nptr;
    assert(isdigit(**buffer));
    long l = strtol(*buffer, &nptr, 10);
    *buffer = nptr;
    return l;
}

#define MEMSZ (1 << 16)


void interpret(FILE *f) {
    char buffer[256];

    // AAA
    // 10X          amask   smask
    // 10A -> AAA & 001   | 100
    //
    // & (set -> 0, X -> 1), | (set -> set, X -> 0)
    uint64_t amask = 0;
    uint64_t smask = 0;
    uint64_t *memory = zmalloc(MEMSZ * sizeof(long));

    while(fgets(buffer, 256, f)) {
        if (strncmp(buffer, "mask", 4) == 0) {
            // update mask
            amask = 0;
            smask = 0;
            for (char *c = buffer + 7; *c != '\n'; c++) {
                amask <<= 1;
                smask <<= 1;
                if (*c == 'X') amask |= 1;
                else           smask |= (*c == '1') ? 1 : 0;
            }
        } else {
            char *c = buffer;
            char **cursor = &c;
            eat_string(cursor, "mem[");
            long address = consume_long(cursor);
            eat_string(cursor, "] = ");
            long value = consume_long(cursor);

            assert(address < MEMSZ);

            memory[address] = (value & amask) | smask;
        }
    }

    uint64_t accumulator = 0;

    for (size_t i = 0; i < (1 << 16); i++) {
        accumulator += memory[i];
        if (memory[i]) printf("%zu: %#lx\n", i, memory[i]);
    }

    printf("final sum: %lu\n", accumulator);
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

    interpret(f);
}
