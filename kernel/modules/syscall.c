#include <basic.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <stdio.h>

int calls = 0;

sysret sys_module_syscall(void) {
    printf("This syscall was defined in a module\n");
    printf("This variable is at %p\n", (void *)&calls);
    printf("It has been called %i times\n", calls++);
    return 0;
}

int init_mod() {
    int num = syscall_register(
        101,
        "module_syscall",
        sys_module_syscall,
        "module_syscall()",
        0
    );
    printf("syscall registered\n");
    return MODINIT_SUCCESS;
}

__USED struct modinfo modinfo = {
    .name = "syscall_mod",
    .init = init_mod,
};
