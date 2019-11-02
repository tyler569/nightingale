
extern "C" {
#include <ng/basic.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <nc/stdio.h>
}

int init_mod(int param) {
    printf("Hello World from this kernel module!\n");
    printf("Imma make a thread now!\n");

    auto thread_fn = []() {
        printf("Hello World from a C++ thread!\n");
        exit_kthread();
    };

    void (*fn)() = thread_fn;

    new_kthread((uintptr_t)fn);
    return 0;
}

