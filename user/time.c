
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <nightingale.h>

int HZ = 50;
#define PER_SECOND(t) (t / HZ)

int main(int argc, char **argv) { 
        if (argc < 2) {
                printf("usage: time [command]");
                exit(0);
        }
        long time = ng_time();

        int child = fork();
        int exit_status;
        if (child == 0) {
                int e = execve(argv[1], argv + 1, NULL);
                perror("errno()");
                exit(1);
        } else {
                waitpid(child, &exit_status, 0);
        }
        long end_time = ng_time();

        printf("time: %li ms\n", (end_time - time) * PER_SECOND(1000));
        return exit_status;;
}

