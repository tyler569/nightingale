#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int forkstuff()
{
    while (true) {
        int child = 0;
        if ((child = fork()) == 0) {
            printf("this is pid:tid %i:%i\n", getpid(), gettid());
            exit(0);
        } else {
            int status;
            waitpid(child, &status, 0);
            printf("waited on child %i with status %i\n", child, status);
        }
    }
}

int main()
{
    for (int i = 0; i < 10; i++) {
        if (!fork())
            forkstuff();
    }

    forkstuff();

    return 0;
}
