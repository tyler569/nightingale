
#include <basic.h>
#include <ng/mod.h>
#include <ng/print.h>

int init_mod(int argument) {
        printf("Hello World from this kernel module!\n");
        return 0;
}

