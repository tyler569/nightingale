#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: create [executable]\n");
        exit(1);
    }

    pid_t pid = create(argv[1]);
    procstate(pid, PS_COPYFDS);
    procstate(pid, PS_SETRUN);
    waitpid(pid, NULL, 0);

    return 0;
}
