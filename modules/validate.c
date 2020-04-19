
#include <basic.h>
#include <ng/mod.h>
#include <nc/stdio.h>

int uninit[1000];
int init[1000] = {1};

enum modinit_status init_mod(struct mod *_) {
        printf("init: %p, init[0]: %i\n", init, init[0]);
        printf("uninit: %p, uninit[0]: %i\n", uninit, uninit[0]);

        return MODINIT_SUCCESS;
}

_used
struct modinfo modinfo = {
        .name = "validate",
        .modinit = init_mod,
};

