
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

// COPYPASTE from sh.c
// this should be somewhere in the C library
int exec(const char *stdio_file, char *const *argv) {
        pid_t child;

        child = fork();
        if (child == -1) {
                perror("fork()");
                return -1;
        }
        if (child == 0) {
                open(stdio_file, O_RDONLY);
                open(stdio_file, O_WRONLY);
                open(stdio_file, O_WRONLY);

                execve(argv[0], argv, NULL);

                printf("init failed to run sh\n");
                exit(1);
        }
        return 0;
}

int main() {
        // do init things

        exec("/dev/serial", (char*[]){"sh", NULL});
        exec("/dev/serial2", (char*[]){"sh", NULL});

        int return_code;
        if (waitpid(-1, &return_code, 0) == -1) {
                // perror("waitpid()");
                return -1;
        }
        return return_code;
}

