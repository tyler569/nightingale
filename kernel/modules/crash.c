#include <ng/common.h>
#include <ng/mod.h>
#include <stdio.h>

int modinit(struct mod *mod)
{
    printf("This module will now crash\n");
    int out;
    asm volatile("movl (0), %0" : "=r"(out));
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "test_mod",
    .init = modinit,
};
