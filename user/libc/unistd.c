
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <errno.h>
#include <ng_syscall.h>
#include "syscall.h"
#include "unistd.h"

noreturn void exit(int status) {
    syscall1(SYS_EXIT, status);
    __unreachable;
}

ssize_t read(int fd, void *data, size_t len) {
    struct syscall_ret ret = syscall3(SYS_READ, (uintptr_t)fd,
                                      (uintptr_t)data, (uintptr_t)len);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

ssize_t write(int fd, const void *data, size_t len) {
    struct syscall_ret ret = syscall3(SYS_WRITE, (uintptr_t)fd,
                                      (uintptr_t)data, (uintptr_t)len);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

pid_t fork(void) {
    struct syscall_ret ret = syscall0(SYS_FORK);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

pid_t getpid(void) {
    struct syscall_ret ret = syscall0(SYS_GETPID);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

pid_t gettid(void) {
    struct syscall_ret ret = syscall0(SYS_GETTID);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int execve(char *program, char **argv, char **envp) {
    struct syscall_ret ret = syscall3(SYS_EXECVE, (uintptr_t)program,
                                      (uintptr_t)argv, (uintptr_t)envp);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

// deprecated pending rework - see kernel/thread.c:sys_wait4
// int wait4(pid_t pid) {
//     syscall1(SYS_WAIT4, (uintptr_t)pid);
// }

int socket(int domain, int type, int protocol) {
    struct syscall_ret ret = syscall3(SYS_SOCKET, (uintptr_t)domain,
                                      (uintptr_t)type, (uintptr_t)protocol);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int strace(bool enable) {
    struct syscall_ret ret = syscall1(SYS_STRACE, (uintptr_t)enable);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int connect(int sock, const struct sockaddr* addr, socklen_t addrlen) {
    struct syscall_ret ret = syscall3(SYS_CONNECT, (uintptr_t)sock,
                                      (uintptr_t)addr, (uintptr_t)addrlen);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    struct syscall_ret ret = syscall3(SYS_BIND, (uintptr_t)sockfd,
                                      (uintptr_t)addr, (uintptr_t)addrlen);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

ssize_t send(int sock, const void* buf, size_t len, int flags) {
    struct syscall_ret ret = syscall4(SYS_SEND, (uintptr_t)sock,
                                      (uintptr_t)buf, (uintptr_t)len,
                                      (uintptr_t)flags);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

ssize_t sendto(int sock, const void* buf, size_t len, int flags,
               const struct sockaddr* remote, socklen_t addrlen) {
    struct syscall_ret ret = syscall6(SYS_SENDTO, (uintptr_t)sock,
                                      (uintptr_t)buf, (uintptr_t)len,
                                      (uintptr_t)flags, (uintptr_t)remote,
                                      (uintptr_t)addrlen);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

ssize_t recv(int sock, void* buf, size_t len, int flags) {
    struct syscall_ret ret = syscall4(SYS_RECV, (uintptr_t)sock,
                                      (uintptr_t)buf, (uintptr_t)len,
                                      (uintptr_t)flags);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

ssize_t recvfrom(int sock, void* buf, size_t len, int flags,
               struct sockaddr* remote, socklen_t* addrlen) {
    struct syscall_ret ret = syscall6(SYS_RECVFROM, (uintptr_t)sock,
                                      (uintptr_t)buf, (uintptr_t)len,
                                      (uintptr_t)flags, (uintptr_t)remote,
                                      (uintptr_t)addrlen);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int waitpid(pid_t pid, int* status, int options) {
    struct syscall_ret ret = syscall3(SYS_WAITPID, (uintptr_t)pid, 
                                      (uintptr_t)status, (uintptr_t)options);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int dup2(int oldfd, int newfd) {
    struct syscall_ret ret = syscall2(SYS_DUP2, (uintptr_t)oldfd,
                                      (uintptr_t)newfd);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}

int uname(struct utsname* name) {
    struct syscall_ret ret = syscall1(SYS_UNAME, (uintptr_t)name);
    if (ret.error) {
        errno = ret.error;
        return -1;
    }
    return ret.value;
}
