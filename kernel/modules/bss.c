#include "ng/common.h"
#include "ng/mod.h"
#include "ng/syscall.h"
#include <stdio.h>

extern int a;
extern int b __attribute__((aligned(64)));

int c;
int d __attribute__((aligned(64)));

int e = 0;
int f __attribute__((aligned(64))) = 0;

int g = 1;
int h __attribute__((aligned(64))) = 1;

extern int i[100];
extern int j[100] __attribute__((aligned(64)));

int k[100];
int l[100] __attribute__((aligned(64)));

int m[100] = { 0 };
int n[100] __attribute__((aligned(64))) = { 0 };

int o[100] = { 1 };
int p[100] __attribute__((aligned(64))) = { 1 };

int init_mod()
{
    printf("ints:   %p %p %p %p %p %p %p %p\n"
           "arrays: %p %p %p %p %p %p %p %p\n",
        (void *)&a, (void *)&b, (void *)&c, (void *)&d, (void *)&e, (void *)&f,
        (void *)&g, (void *)&h, (void *)&i, (void *)&j, (void *)&k, (void *)&l,
        (void *)&m, (void *)&n, (void *)&o, (void *)&p);
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "bss_tester",
    .init = init_mod,
};
