#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile long number_of_times = 0;

int thread_func(void *_arg) {
    int tid = gettid();
    while (true) {
        for (int i = 0; i < 500000; i++)
            ;

        number_of_times += 1;
        printf("(%i:%li) ", tid, number_of_times);
        if (number_of_times > 50) break;
    }

    exit(0);
}

int main() {
    for (int i = 0; i < 10; i++) {
        char *new_stack = malloc(0x1000);
        clone(thread_func, NULL, new_stack + 0x1000, 0);
    }

    exit(0);
}
