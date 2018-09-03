
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <errno.h>
#include <ng_syscall.h>
#include "unistd.h"

uintptr_t syscall0(int syscall_num) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) : "0"(syscall_num));
    return ret;
}

uintptr_t syscall1(int syscall_num, uintptr_t arg1) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) : "0"(syscall_num), "D"(arg1));
    return ret;
}

uintptr_t syscall3(int syscall_num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3));
    return ret;
}


void debug_print(const char *message) {
    syscall1(SYS_DEBUGPRINT, (uintptr_t)message);
}

__attribute__((noreturn))
void exit(int status) {
    syscall1(SYS_EXIT, 0);
    __builtin_unreachable();
}

size_t read(int fd, void *data, size_t len) {
    size_t ret;
    ret = (size_t)syscall3(SYS_READ, (uintptr_t)fd, (uintptr_t)data, (uintptr_t)len);
    return ret;
}

size_t write(int fd, const void *data, size_t len) {
    size_t ret;
    ret = (size_t)syscall3(SYS_WRITE, (uintptr_t)fd, (uintptr_t)data, (uintptr_t)len);
    return ret;
}

pid_t fork(void) {
    pid_t ret;
    ret = (pid_t)syscall0(SYS_FORK);
    return ret;
}

void top(void) {
    syscall0(SYS_TOP);
}

pid_t getpid(void) {
    syscall0(SYS_GETPID);
}

pid_t gettid(void) {
    syscall0(SYS_GETTID);
}

void execve(char *program, char **argv, char **envp) {
    syscall3(SYS_EXECVE, (uintptr_t)program, (uintptr_t)argv, (uintptr_t)envp);
}

int wait4(pid_t pid) {
    syscall1(SYS_WAIT4, (uintptr_t)pid);
}

