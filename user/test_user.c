
#include <stddef.h>
#include <stdint.h>

#define SYS_DEBUGPRINT 1
#define SYS_EXIT 2

int errno;
char *message = "This is a message that I want to print out\n";

int syscall1(int syscall_num, uintptr_t arg1) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) : "0"(syscall_num), "D"(arg1));
    return ret;
}

int _start() {
    syscall1(SYS_DEBUGPRINT, (uintptr_t)message);
    syscall1(SYS_EXIT, 0);
}

