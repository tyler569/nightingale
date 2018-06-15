
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

int exec(char *program) {
    pid_t child;
    if ((child = fork()) == 0) {
        execve(program, NULL, NULL);

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
        printf("child is %i\n", child);
        printf("would wait4 here\n");
    }
}

size_t read_line(char *buf) {
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

    char command[64] = {0};
    size_t ix = 0;
    char c;

    while (true) {
        printf("$ ");

        read_line(command);

        //printf("\n");

        if (command[0] == 0)
            continue;

        if (strncmp(command, "echo", 4) == 0) {
            printf("%s\n", command + 5);
        } else if (strncmp(command, "fork", 4) == 0) {
            pid_t child;
            if ((child = fork()) == 0) {
                printf("this is the child - pid:%i, tid:%i\n", getpid(), gettid());
                exit(0);
            } else {
                printf("child pid: %i\n", child);
            }
        } else if (strncmp(command, "top", 3) == 0) {
            top();
        } else if (strncmp(command, "crash", 5) == 0) {
            printf("%c\n", *(char *)0);
        } else if (strncmp(command, "getpid", 6) == 0) {
            printf("%i\n", getpid());
        } else if (strncmp(command, "gettid", 6) == 0) {
            printf("%i\n", gettid());
        } else {
            exec(command);
        }

        command[0] = 0;
    }

    return 0;
}

int _start() {
    int status = main();
    exit(status);
}

