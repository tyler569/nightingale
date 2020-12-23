#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile long number_of_times = 0;
bool go_slow = false;

void slow() {
    for (int i = 0; i < 1000000; i++) asm("");
}

int thread_func(void *_arg) {
    int tid = gettid();
    while (true) {
        if (go_slow) slow();

        number_of_times++;
        printf("tid %i, times %li\n", tid, number_of_times);
        if (number_of_times > 20) break;
        yield();
    }

    exit(0);
}

#define STACK_SIZE 0x200

int main() {
    for (int i = 0; i < 10; i++) {
        char *new_stack = malloc(STACK_SIZE);
        clone(thread_func, new_stack + STACK_SIZE, 0, NULL);
    }

    exit(0);
}
