#include <basic.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <stdio.h>

int calls = 0;

sysret sys_module_syscall(void) {
    printf("This syscall was defined in a module\n");
    printf("It has been called %i times\n", calls);
    return 0;
}

enum modinit_status init_mod() {
    int num = syscall_register(101, sys_module_syscall, "module_syscall()", 0);
    printf("syscall registered\n");
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "validate",
    .init = init_mod,
};
