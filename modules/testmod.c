#include <basic.h>
#include <ng/mod.h>
#include <stdio.h>

enum modinit_status modinit(struct mod *mod) {
    printf("Hello World from this kernel module!\n");
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "test_mod",
    .init = modinit,
};
