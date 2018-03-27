
#include <stddef.h>
#include <stdint.h>

#define SYS_DBGPRINT 0
#define SYS_EXIT 1

int errno;
char *message = "This is a message that I want to print out\n";

int syscall(int syscall_num, uintptr_t arg1) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) : "a"(syscall_num), "b"(arg1));
    return ret;
}

int _start() {
    syscall(SYS_DBGPRINT, (uintptr_t)message);
    syscall(SYS_EXIT, 0);
}

