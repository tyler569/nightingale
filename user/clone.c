#include <sched.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>

atomic_int number_of_times = 0;
atomic_int threads_done = 0;
bool go_slow = false;

noreturn void exit_thread(int code);

void slow() {
    for (int i = 0; i < 1000000; i++)
        asm ("");
}

int thread_func(void *_arg) {
    int tid = gettid();
    while (true) {
        if (go_slow)
            slow();

        atomic_fetch_add(&number_of_times, 1);
        printf(
            "tid %i, times %li\n",
            tid,
            atomic_load(&number_of_times)
        );
        if (atomic_load(&number_of_times) > 20)
            break;
        yield();
    }

    atomic_fetch_add(&threads_done, 1);
    exit_thread(0);
}

#define STACK_SIZE 0x2000

int main() {
    for (int i = 0; i < 10; i++) {
        char *new_stack = malloc(STACK_SIZE);
        clone(thread_func, new_stack + STACK_SIZE, 0, NULL);
    }

    while (atomic_load(&threads_done) < 10)
        yield();
    exit(0);
}
