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

long map(long i, long xmask) {
    long bit = 0;
    long out = 0;

    for (long n = 0; n < 64; n++) {
        while (!(xmask & (1ul << bit))) bit++;

        if (i & (1ul << n)) {
            out |= (1ul << bit);
        }

        xmask &= ~(1ul << bit);

        if (xmask == 0) break;
    }

    return out;
}

#define MEMSZ (1 << 17)

long *perfect_hash_cache;
long hash(long addr) {
    for (size_t i = 0; i < MEMSZ; i++) {
        if (addr == perfect_hash_cache[i]) return i;
        if (perfect_hash_cache[i] == -1) {
            perfect_hash_cache[i] = addr;
            return i;
        }
    }
    assert(0 && "filled perfect cache");
}


void interpret(FILE *f) {
    char buffer[256];

    perfect_hash_cache = malloc(MEMSZ * sizeof(long));
    memset(perfect_hash_cache, 0xFF, MEMSZ * sizeof(long));

    // AAA
    // 10X          amask   smask
    // 10A -> AAA & 001   | 100
    //
    // & (set -> 0, X -> 1), | (set -> set, X -> 0)
    long zmask = 0;
    long omask = 0;
    long xmask = 0;
    long nx = 0;
    uint64_t *memory = zmalloc(MEMSZ * sizeof(long));

    while(fgets(buffer, 256, f)) {
        if (strncmp(buffer, "mask", 4) == 0) {
            zmask = 0;
            omask = 0;
            xmask = 0;
            nx = 0;
            for (char *c = buffer + 7; *c != '\n'; c++) {
                zmask <<= 1;
                omask <<= 1;
                xmask <<= 1;
                switch (*c) {
                case '0': zmask |= 1; break;
                case '1': omask |= 1; break;
                case 'X': xmask |= 1; nx++; break;
                default: assert(0 && "unreachable");
                }
            }
        } else {
            char *c = buffer;
            char **cursor = &c;
            eat_string(cursor, "mem[");
            long address = consume_long(cursor);
            eat_string(cursor, "] = ");
            long value = consume_long(cursor);

            long base_address = (address & zmask) | omask;
            for (long i = 0; i < (1 << nx); i++) {
                long addr = base_address | map(i, xmask);
                printf("addr: %zx %zx %zx\n", addr, hash(addr), value);
                memory[hash(addr)] = value;
            }
        }
    }

    uint64_t accumulator = 0;

    for (size_t i = 0; i < MEMSZ; i++) {
        accumulator += memory[i];
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

    // for (int i = 0; i < 16; i++) {
    //     printf("map(%i, 0x10101010): %#lx\n", i, map(i, 0x10101010));
    // }

    interpret(f);
}
