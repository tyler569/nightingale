
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char **argv) {
        if (argc < 2) {
                printf("Not enough arguments\n");
                exit(1);
        }

        pid_t pid = atoi(argv[1]);
        kill(pid, SIGINT);
        return EXIT_SUCCESS;
}

