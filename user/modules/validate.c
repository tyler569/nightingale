
#include <basic.h>
#include <ng/mod.h>
#include <nc/stdio.h>

int uninit[1000];
int init[1000] = {1};

int init_mod(int argument) {
        printf("uninit: %p, uninit[0]: %i\n", uninit, uninit[0]);
        printf("init: %p, init[0]: %i\n", init, init[0]);

        return 0;
}

