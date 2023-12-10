#include <ng/mod.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <stdio.h>

[[noreturn]] void mod_kthread(void *)
{
    printf("This is the thread!\n");
    while (true)
        sleep_thread(seconds(1));
}

int init_mod(struct mod *_)
{
    printf("Hello World from this kernel module!\n");
    printf("Imma make a thread now!\n");
    kthread_create(mod_kthread, nullptr);
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "thread_mod",
    .init = init_mod,
};
