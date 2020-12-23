#include <sched.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile long number_of_times = 0;
atomic_int threads_done = 0;
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

    threads_done++;
    exit(0);
}

#define STACK_SIZE 0x2000

int main() {
    for (int i = 0; i < 10; i++) {
        char *new_stack = malloc(STACK_SIZE);
        clone(thread_func, new_stack + STACK_SIZE, 0, NULL);
    }

    while (threads_done < 10) yield();
    exit(0);
}
