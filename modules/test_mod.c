
#include <basic.h>
#include <ng/mod.h>
#include <stdio.h>

enum modinit_status modinit(struct mod *_) {
        printf("Hello World from this kernel module!\n");
        return MODINIT_SUCCESS;
}

_used
struct modinfo modinfo = {
        .name = "test_mod",
        .modinit = modinit,
};

