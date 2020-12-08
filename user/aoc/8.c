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

size_t find_loop(struct instruction *program, size_t len) {
    size_t pc = 0;
    long acc = 0;

    char *executed = calloc(len, 1);

    while (true) {
        struct instruction *e = &program[pc];
        if (executed[pc]) return acc;
        executed[pc]++;
        switch (e->opcode) {
        case NOP: pc++; break;
        case ACC: acc += e->argument; pc++; break;
        case JMP: pc += e->argument; break;
        default: assert(0 && "bad instruction");
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

    size_t len = line_count(file);

    struct instruction *program = assemble(file, len);
    printf("first loop: %li\n", find_loop(program, len));
}
