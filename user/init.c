
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
        return child;
}

void run_sh_forever(const char *device) {
        while (true) {
                int child = exec(device, (char *[]){"sh", NULL});
                int return_code;
                waitpid(child, &return_code, 0);
        }
}

#define SERIAL2 0

int main() {
        // do init things

#if SERIAL2
        if (fork())
                run_sh_forever("/dev/serial");
        run_sh_forever("/dev/serial2");
#else
        run_sh_forever("/dev/serial");
#endif

        assert(0);
}

