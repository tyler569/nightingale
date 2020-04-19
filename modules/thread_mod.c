
#include <basic.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <nc/stdio.h>

void mod_kthread() {
        printf("This is the thread!\n");
        exit_kthread();
}

enum modinit_status init_mod(struct mod *_) {
        printf("Hello World from this kernel module!\n");
        printf("Imma make a thread now!\n");
        new_kthread((uintptr_t)mod_kthread);
        return MODINIT_SUCCESS;
}

_used
struct modinfo modinfo = {
        .name = "thread_mod",
        .modinit = init_mod,
};

