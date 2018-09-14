
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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
        int return_code;
        waitpid(child, &return_code, 0);
        return return_code;
    }
}

int main() {
    printf("Hello World from %s %i!\n", "ring", 3);

    // do init things

    while (true) {
        char* argv0 = NULL;
        exec("sh", &argv0);
    }
}

