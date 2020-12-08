#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strcpyto(char *dest, const char *source, char delim) {
    while (*source && *source != delim) { *dest++ = *source++; }
    *dest = 0;
    return (char *)source;
}

size_t line_count(FILE *f) {
    size_t nlines = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') nlines++;
    }
    fseek(f, 0, SEEK_SET);
    return nlines;
}

enum opcode {
    JMP,
    ACC,
    NOP,
};

struct instruction {
    enum opcode opcode;
    long argument;
};

struct instruction *assemble(FILE *stream, size_t count) {
    char buffer[32];

    struct instruction *program = malloc(sizeof(struct instruction) * count);
    size_t index = 0;
    
    while (fgets(buffer, 32, stream)) {
        struct instruction *instr = &program[index];

        if (strncmp(buffer, "jmp", 3) == 0) {
            instr->opcode = JMP;
        } else if (strncmp(buffer, "acc", 3) == 0) {
            instr->opcode = ACC;
        } else if (strncmp(buffer, "nop", 3) == 0) {
            instr->opcode = NOP;
        } else {
            assert(0 && "bad opcode");
        }

        instr->argument = strtol(&buffer[4], NULL, 10);

        index++;
    }

    return program;
}

size_t check_loop(struct instruction *program, size_t len) {
    size_t pc = 0;
    long acc = 0;

    char *executed = calloc(len, 1);

    while (true) {
        if (pc == len - 1) return acc;
        struct instruction *e = &program[pc];
        if (executed[pc]) return 0;
        executed[pc]++;
        switch (e->opcode) {
        case NOP: pc++; break;
        case ACC: acc += e->argument; pc++; break;
        case JMP: pc += e->argument; break;
        default: assert(0 && "bad instruction");
        }
    }

    free(executed);
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

    size_t len = line_count(file);
    struct instruction *program = assemble(file, len);
    struct instruction *program_copy = malloc(sizeof(struct instruction) * len);

    for (size_t i = 0; i < len; i++) {
        memcpy(program_copy, program, sizeof(struct instruction) * len);
        if (program_copy[i].opcode == JMP) {
            program_copy[i].opcode = NOP;
        } else if (program_copy[i].opcode == NOP) {
            program_copy[i].opcode = JMP;
        }

        long acc;
        if ((acc = check_loop(program_copy, len))) {
            printf("Found solution @ %zu-- acc: %li\n", i, acc);
            break;
        }
    }
}
