#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

int thread_func(void *_arg)
{
    while (true) {
        int child = 0;
        if ((child = fork()) == 0) {
            printf("this is %i\n", getpid());
            exit(0);
        } else {
            int status;
            waitpid(child, &status, 0);
            printf("waited on child %i with status %i\n", child, status);
        }
    }
}

#define STACK_SIZE 0x2000

int main()
{
    for (int i = 0; i < 10; i++) {
        char *new_stack = malloc(STACK_SIZE);
        clone(thread_func, new_stack + STACK_SIZE, 0, NULL);
    }

    while (true)
        sleep(1000);
    exit(0);
}
