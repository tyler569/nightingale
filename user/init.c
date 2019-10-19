
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// COPYPASTE from sh.c
// this should be somewhere in the C library
int exec(const char *program, char *const *argv) {
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
                        printf(
                            "%s is not executable or is not a valid format\n",
                            program);
                        break;
                default:
                        printf("An unknown error occured running %s\n",
                               program);
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

int main() {
        printf("Hello World from %s %i!\n", "ring", 3);

        // do init things

        while (true) {
                exec("/bin/sh", (char*[]){"sh", NULL});
        }
}

