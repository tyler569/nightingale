
#include <basic.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <nc/stdio.h>

void mod_kthread() {
        printf("This is the thread!\n");
        kthread_exit();
}

enum modinit_status init_mod(struct mod *_) {
        printf("Hello World from this kernel module!\n");
        printf("Imma make a thread now!\n");
        kthread_create(mod_kthread, NULL);
        return MODINIT_SUCCESS;
}

_used
struct modinfo modinfo = {
        .name = "thread_mod",
        .modinit = init_mod,
};

