
#include <stdio.h>
#include "setjmp.h"

int main() {
    jmp_buf foo;
    setjmp(foo);

    printf("&main is %p\n", main);

    for (int i=0; i<8; i++) {
        printf("%i: %lx\n", i, foo->__array[i]);
    }

    printf("rsp: %lx\n", foo->__regs.rsp);
    printf("rip: %lx\n", foo->__regs.rip);
}
