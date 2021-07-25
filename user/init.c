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
        perror("execve");
        exit(127);
    }
    return child;
}

void run_sh_forever(const char *device) {
    while (true) {
        int child = exec(device, (char *[]){"/bin/sh", NULL});
        int return_code;
        while (true) {
            int pid = waitpid(child, &return_code, 0);
            if (pid < 0 && errno == EINTR) continue;
            if (pid < 0) {
                perror("waitpid");
                exit(1);
            }
            break;
        }
        if (return_code) assert("init failed to start the shell" && 0);
    }
}

int cleanup_children(void *arg) {
    (void)arg;
    // NOTE: there are no open files here, if you need to print anything in
    // this thread, you need to open something. This can potentially mess up
    // the opens for the `exec()` call, so be careful.
    while (true) {
        int status;
        pid_t pid = waitpid(-1, &status, 0);
    }
}

char ch_stack[0x1000];

int main() {
    // TODO: do init things
    clone(cleanup_children, &ch_stack[0] + 0x1000, 0, 0);
    run_sh_forever("/dev/serial");
    assert(0);
}
