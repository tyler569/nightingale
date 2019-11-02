
#include <ng/basic.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <nc/stdio.h>

void mod_kthread() {
        printf("This is the thread!\n");
        exit_kthread();
}

int init_mod(int param) {
        printf("Hello World from this kernel module!\n");
        printf("Imma make a thread now!\n");
        new_kthread((uintptr_t)mod_kthread);
        return 0;
}

