
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
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

uintptr_t syscall2(int syscall_num, uintptr_t arg1, uintptr_t arg2) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2));
    return ret;
}

uintptr_t syscall3(int syscall_num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3));
    return ret;
}

uintptr_t syscall4(int syscall_num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
                   uintptr_t arg4) {
    uintptr_t ret;
    asm volatile ("int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4));
    return ret;
}

uintptr_t syscall5(int syscall_num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
                   uintptr_t arg4, uintptr_t arg5) {
    uintptr_t ret;
    asm volatile ("mov %7, %%r8\n\t"
                  "int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4),
                  "r"(arg5));
    return ret;
}

uintptr_t syscall6(int syscall_num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
                   uintptr_t arg4, uintptr_t arg5, uintptr_t arg6) {
    uintptr_t ret;
    asm volatile ("mov %7, %%r8\n\t"
                  "mov %8, %%r9\n\t"
                  "int $0x80" : "=a"(ret), "=c"(errno) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4),
                  "r"(arg5), "r"(arg6));
    return ret;
}


void debug_print(const char *message) {
    syscall1(SYS_DEBUGPRINT, (uintptr_t)message);
}

__attribute__((noreturn))
void exit(int status) {
    syscall1(SYS_EXIT, status);
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

int socket(int domain, int type, int protocol) {
    syscall3(SYS_SOCKET, (uintptr_t)domain, (uintptr_t)type,
             (uintptr_t)protocol);
}

int strace(bool enable) {
    syscall1(SYS_STRACE, (uintptr_t)enable);
}

int connect(int sock, const struct sockaddr* addr, socklen_t addrlen) {
    syscall3(SYS_CONNECT, (uintptr_t)sock, (uintptr_t)addr, (uintptr_t)addrlen);
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    syscall3(SYS_BIND, (uintptr_t)sockfd, (uintptr_t)addr, (uintptr_t)addrlen);
}

ssize_t send(int sock, const void* buf, size_t len, int flags) {
    syscall4(SYS_SEND, (uintptr_t)sock, (uintptr_t)buf, (uintptr_t)len,
             (uintptr_t)flags);
}

ssize_t sendto(int sock, const void* buf, size_t len, int flags,
               const struct sockaddr* remote, socklen_t addrlen) {
    syscall6(SYS_SENDTO, (uintptr_t)sock, (uintptr_t)buf, (uintptr_t)len,
             (uintptr_t)flags, (uintptr_t)remote, (uintptr_t)addrlen);
}

ssize_t recv(int sock, void* buf, size_t len, int flags) {
    syscall4(SYS_RECV, (uintptr_t)sock, (uintptr_t)buf, (uintptr_t)len,
            (uintptr_t)flags);
}

ssize_t recvfrom(int sock, void* buf, size_t len, int flags,
               struct sockaddr* remote, socklen_t* addrlen) {
    syscall6(SYS_RECVFROM, (uintptr_t)sock, (uintptr_t)buf, (uintptr_t)len,
             (uintptr_t)flags, (uintptr_t)remote, (uintptr_t)addrlen);
}
