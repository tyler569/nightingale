
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <nightingale.h>

int main(int argc, char **argv) {
        if (argc < 2) {
                printf("usage: create [executable]\n");
                exit(1);
        }

        pid_t pid = create(argv[1]);
        procstate(pid, PS_COPYFDS);
        procstate(pid, PS_SETRUN);

        return 0;
}

