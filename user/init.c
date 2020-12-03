#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int exec(const char *stdio_file, char **argv) {
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

        printf("Welcome to Nightingale\n");
        execve(argv[0], argv, NULL);

        printf("init failed to run sh\n");
        exit(127);
    }
    return child;
}

void run_sh_forever(const char *device) {
    while (true) {
        int child = exec(device, (char *[]){"/bin/sh", NULL});
        int return_code;
        waitpid(child, &return_code, 0);
        assert("init failed to start the shell" && (return_code != 127));
    }
}

int main() {
    // TODO: do init things
    run_sh_forever("/dev/serial");
    assert(0);
}
