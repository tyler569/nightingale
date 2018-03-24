
#include <stddef.h>
#include <stdint.h>

#define SYS_EXIT 1

int errno;

int syscall(int syscall_num, uintptr_t arg1) {
    uintptr_t ret;

    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) : "A"(syscall_num));

    return ret;
}

int _start() {
    syscall(SYS_EXIT, 0);
}

