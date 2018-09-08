
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int fork_test() {
    printf("This is a test fork from pid: TBD\n");
    exit(0);
}

int exec(char *program, char **argv) {
    pid_t child;

#ifdef __ng_program_args_debug
    printf("program: %#lx, argv: %#lx\n", program, argv);
    printf("executing %s\n", program);

    for (int i=0; i<32; i++) {
        if (!argv[i])
            break;
        printf("argument %i is %s\n", i, argv[i]);
    }
#endif

    if ((child = fork()) == 0) {
        execve(program, argv, NULL);

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
        return wait4(child);
    }
}

size_t read_line(char *buf, size_t max_len) {
    size_t ix = 0;
    char c;

    while (true) {
        c = getchar();

        if (c == 0x7f && ix > 0) { // backspace
            ix -= 1;
            buf[ix] = '\0';
            printf("\x08 \x08");
            continue;
        } else if (c == 0x7f) {
            continue;
        } else if (c == '\n') { // newline
            printf("\n");
            break;
        } else if (!isprint(c)) {
            continue;
        }

        buf[ix++] = c;
        buf[ix] = '\0';
        printf("%c", c);
    }

    return ix;
}

int main() {
    printf("Hello World from %s %i!\n", "ring", 3);
    printf("Nightingale init debug shell:\n");
    // strace(true);

    while (true) {
        printf("$ ");

        char cmdline[256] = {0};
        char *args[32] = {0};

        read_line(cmdline, 256);

        char *c = cmdline;
        char *arg_start = cmdline;
        size_t arg = 0;

        bool was_space = true;
        bool is_space = false;
        while (*c != 0) {
            is_space = isblank(*c);
            if (!is_space && was_space) {
                args[arg++] = c;
            } else if (is_space) {
                *c = '\0';
            }
            was_space = is_space;
            c += 1;

            // TODO: "" and ''
        }

        args[arg] = NULL;

        if (cmdline[0] == 0)
            continue;

        printf("exited - return: %i\n", exec(args[0], &args[1]));

        cmdline[0] = 0;
    }

    return 0;
}

