
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
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
        /*
        printf("child is %i\n", child);
        printf("would wait4 here\n");
        */
    }
}

size_t read_line(char *buf, size_t max_len) {
    size_t ix = 0;
    char c;

    while (true) {
        read(4, &c, 1);

        if (c == 0x7f && ix > 0) { // backspace
            ix -= 1;
            buf[ix] = '\0';
            printf("\x08");
            continue;
        } else if (c == 0x7f) {
            continue;
        }

        if (c == '\n') { // newline
            printf("\n");
            break;
        }

        buf[ix++] = c;
        buf[ix] = '\0';
        printf("%c", c);
    }

    return ix;
}

int main() {
    printf("Hello World from %s %i!\n", "ring", 3);

    while (true) {
        printf("$ ");

        char cmdline[256] = {0};
        char *args[32] = {0};

        read_line(cmdline, 256);

        char *ptr = cmdline;
        char *arg_start = cmdline;
        size_t arg = 0;

        while (*ptr != 0) {
            if (ptr[0] != ' ' && ptr[1] == ' ') {
                args[arg++] = arg_start;
                ptr[1] = '\0';
                ptr += 2;
                arg_start = ptr;
            } else if (ptr[0] != ' ' && ptr[1] == '\0') {
                args[arg++] = arg_start;
                break;
            }
            ptr += 1;
            // TODO: does not account for multiple whitespace chars correctly
            // TODO: quoted strings as a single argument
            // TODO: apparently this can't see spaces after one-letter arguments
        }

        args[arg] = NULL;

        if (cmdline[0] == 0)
            continue;

        exec(args[0], &args[1]);

        cmdline[0] = 0;
    }

    return 0;
}

int _start() {
    int status = main();
    exit(status);
}

