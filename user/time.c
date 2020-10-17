
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <nightingale.h>

int main(int argc, char **argv) { 
        if (argc < 2) {
                printf("usage: time [command]");
                exit(0);
        }
        long time = xtime();

        int child = fork();
        int exit_status;
        if (child == 0) {
                int e = execve(argv[1], argv + 1, NULL);
                perror("errno()");
                exit(1);
        } else {
                waitpid(child, &exit_status, 0);
        }
        long end_time = xtime();

        printf("time: %li ms\n", (end_time - time) * (1000 / CLOCKS_PER_SEC));
        return exit_status;;
}

