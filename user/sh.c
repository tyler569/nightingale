
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int exec(char *program, char **argv) {
    pid_t child;

    child = fork();
    if (child == -1) {
        perror("fork()");
        return -1;
    }
    if (child == 0) {
        execve(program, argv, NULL);

        // getting here constitutes failure
        switch (errno) {
        case ENOENT:
            printf("%s does not exist\n", program);
            break;
        case ENOEXEC:
            printf("%s is not executable or is not a valid format\n", program);
            break;
        default:
            printf("An unknown error occured running %s\n", program);
        }

        exit(127);
    } else {
        int return_code;
        if (waitpid(child, &return_code, 0) == -1) {
            perror("waitpid()");
            return -1;
        }
        return return_code;
    }
}

void clear_line(char* buf, size_t* ix) {
    while (*ix > 0) {
        *ix -= 1;
        buf[*ix] = '\0';
        printf("\x08 \x08");
    }
}

void backspace(char* buf, size_t* ix) {
    if (ix == 0)  return;
    *ix -= 1;
    buf[*ix] = '\0';
    printf("\x08 \x08");
}

void load_line(char* buf, size_t* ix, char* new_line) {
    clear_line(buf, ix);
    while (*new_line) {
        buf[*ix] = *new_line;
        printf("%c", *new_line);
        new_line += 1;
        *ix += 1;
    }
}

typedef struct hist hist;
struct hist {
    hist* previous;
    hist* next;
    char* history_line;
};

hist hist_base = {0};
hist* hist_top = 0;

void store_history_line(char* line_to_store, size_t len) {
    char* line = malloc(len + 5);
    strcpy(line, line_to_store);
    hist* new_hist_ent = malloc(sizeof(hist));
    new_hist_ent->history_line = line;

    if (!hist_top) { 
        hist_top = &hist_base;
    }

    new_hist_ent->previous = hist_top;
    hist_top->next = new_hist_ent;
    hist_top = new_hist_ent;
}

void load_history_line(char* buf, size_t* ix, hist* current) {
    clear_line(buf, ix);
    if (!current->history_line)
        return;
    load_line(buf, ix, current->history_line);
}

size_t read_line(char *buf, size_t max_len) {
    size_t ix = 0;
    int readlen = 0;
    char cb[256] = {0};
    hist* current = hist_top;

    while (true) {
        readlen = read(stdin, cb, 256);
        if (readlen == -1) {
            perror("read()");
            return -1;
        }

        for (int i=0; i<readlen; i++) {
            char escape_status = 0;
            char c = cb[i];

            switch (c) {
            case 0x7f: // backspace
                backspace(buf, &ix);  continue;
            case 0x0b: // ^K
                clear_line(buf, &ix);  continue;
            case 0x0c: // ^L
                load_line(buf, &ix, "heapdbg both");  continue;
            case 0x0e: // ^N
                load_history_line(buf, &ix, current);
                if (current->previous) current = current->previous;
                continue;
            case '\n':
                goto done;
            }

            if (!isprint(c)) {
                printf("(%hhx)", c);
                continue;
            }

            buf[ix++] = c;
            buf[ix] = '\0';
            printf("%c", c);
            cb[i] = 0;
        }
    }

done:
    if (ix > 0) {
        // printf("storing history\n");
        store_history_line(buf, ix);
    }
    printf("\n");
    return ix;
}

int main() {
    printf("Nightingale shell\n");

    while (true) {
        printf("$ ");

        char cmdline[256] = {0};
        char *args[32] = {0};

        if (read_line(cmdline, 256) == -1) {
            return 1;
        }

        char *c = cmdline;
        size_t arg = 0;

        bool was_space = true;
        bool is_space = false;
        int in_quote = '\0';
        while (*c != 0) {
            is_space = isblank(*c);
            if (!is_space && was_space) {
                args[arg++] = c;
            } else if (is_space) {
                *c = '\0';
            }

            was_space = is_space;
            c += 1;
        }

        args[arg] = NULL;

        if (cmdline[0] == 0)
            continue;

        if (strncmp("history", cmdline, 7) == 0) {
            hist* hl = hist_top;
            for (; hl->history_line; hl = hl->previous) {
                printf("%s\n", hl->history_line);
            }
            continue;
        }

        if (strncmp("exit", cmdline, 4) == 0) {
            return 0;
        }

        printf("-> %i ", exec(args[0], &args[1]));

        cmdline[0] = 0;
    }

    return 0;
}

