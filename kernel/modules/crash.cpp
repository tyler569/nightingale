#include <ng/mod.h>
#include <stdio.h>

int modinit(struct mod *)
{
    printf("This module will now crash\n");
    int out;
    __asm__ volatile("movl (0), %0" : "=r"(out));
    return MODINIT_SUCCESS;
}

__USED modinfo modinfo = {
    .name = "test_mod",
    .init = modinit,
};
