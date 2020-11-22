#include <basic.h>
#include <ng/mod.h>
#include <ng/thread.h>
#include <stdio.h>

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

__USED struct modinfo modinfo = {
    .name = "thread_mod",
    .init = init_mod,
};
