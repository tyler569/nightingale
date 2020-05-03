
#include <errno.h>
#include <dirent.h>
#include <syscall_consts.h>
#include <nightingale.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include "syscall.h"

static inline int is_error(intptr_t ret) {
        return (ret < 0 && ret > -0x1000);
}

noreturn void exit(int status) {
        syscall1(NG_EXIT, status);
        __builtin_unreachable();
}

noreturn void exit_group(int status) {
        syscall1(NG_EXIT_GROUP, status);
        __builtin_unreachable();
}

#define RETURN_OR_SET_ERRNO(ret) \
        if (!is_error(ret)) { \
                return ret; \
        } else { \
                errno = -ret; \
                return -1; \
        }

ssize_t read(int fd, void *data, size_t len) {
        intptr_t ret =
            syscall3(NG_READ, (intptr_t)fd, (intptr_t)data, (intptr_t)len);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t write(int fd, const void *data, size_t len) {
        intptr_t ret =
            syscall3(NG_WRITE, (intptr_t)fd, (intptr_t)data, (intptr_t)len);
        RETURN_OR_SET_ERRNO(ret);
}

pid_t fork(void) {
        intptr_t ret = syscall0(NG_FORK);
        RETURN_OR_SET_ERRNO(ret);
}

pid_t getpid(void) {
        intptr_t ret = syscall0(NG_GETPID);
        RETURN_OR_SET_ERRNO(ret);
}

pid_t gettid(void) {
        intptr_t ret = syscall0(NG_GETTID);
        RETURN_OR_SET_ERRNO(ret);
}

int execve(const char *program, char *const *argv, char *const *envp) {
        intptr_t ret = syscall3(NG_EXECVE, (intptr_t)program,
                                          (intptr_t)argv, (intptr_t)envp);
        RETURN_OR_SET_ERRNO(ret);
}

// deprecated pending rework - see kernel/thread.c:sys_wait4
// int wait4(pid_t pid) {
//     syscall1(NG_WAIT4, (intptr_t)pid);
// }

int socket(int domain, int type, int protocol) {
        intptr_t ret = syscall3(NG_SOCKET, (intptr_t)domain,
                                          (intptr_t)type, (intptr_t)protocol);
        RETURN_OR_SET_ERRNO(ret);
}

int strace(bool enable) {
        intptr_t ret = syscall1(NG_STRACE, (intptr_t)enable);
        RETURN_OR_SET_ERRNO(ret);
}

int connect(int sock, const struct sockaddr *addr, socklen_t addrlen) {
        intptr_t ret = syscall3(NG_CONNECT, (intptr_t)sock,
                                          (intptr_t)addr, (intptr_t)addrlen);
        RETURN_OR_SET_ERRNO(ret);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        intptr_t ret = syscall3(NG_BIND, (intptr_t)sockfd,
                                          (intptr_t)addr, (intptr_t)addrlen);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t send(int sock, const void *buf, size_t len, int flags) {
        intptr_t ret =
            syscall4(NG_SEND, (intptr_t)sock, (intptr_t)buf, (intptr_t)len,
                     (intptr_t)flags);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t sendto(int sock, const void *buf, size_t len, int flags,
               const struct sockaddr *remote, socklen_t addrlen) {
        intptr_t ret = syscall6(
            NG_SENDTO, (intptr_t)sock, (intptr_t)buf, (intptr_t)len,
            (intptr_t)flags, (intptr_t)remote, (intptr_t)addrlen);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t recv(int sock, void *buf, size_t len, int flags) {
        intptr_t ret =
            syscall4(NG_RECV, (intptr_t)sock, (intptr_t)buf, (intptr_t)len,
                     (intptr_t)flags);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t recvfrom(int sock, void *buf, size_t len, int flags,
                 struct sockaddr *remote, socklen_t *addrlen) {
        intptr_t ret = syscall6(
            NG_RECVFROM, (intptr_t)sock, (intptr_t)buf, (intptr_t)len,
            (intptr_t)flags, (intptr_t)remote, (intptr_t)addrlen);
        RETURN_OR_SET_ERRNO(ret);
}

int waitpid(pid_t pid, int *status, enum wait_options options) {
        intptr_t ret = syscall3(NG_WAITPID, pid, (intptr_t)status, options);
        RETURN_OR_SET_ERRNO(ret);
}

int dup2(int oldfd, int newfd) {
        intptr_t ret =
            syscall2(NG_DUP2, (intptr_t)oldfd, (intptr_t)newfd);
        RETURN_OR_SET_ERRNO(ret);
}

int uname(struct utsname *name) {
        intptr_t ret = syscall1(NG_UNAME, (intptr_t)name);
        RETURN_OR_SET_ERRNO(ret);
}

int yield(void) {
        intptr_t ret = syscall0(NG_YIELD);
        RETURN_OR_SET_ERRNO(ret);
}

int open(const char *name, int flags, int mode) {
        intptr_t ret = syscall3(NG_OPEN, (intptr_t)name, flags, mode);
        RETURN_OR_SET_ERRNO(ret);
}

off_t seek(int fd, off_t offset, int whence) {
        intptr_t ret = syscall3(NG_SEEK, (intptr_t)fd,
                                          (intptr_t)offset, (intptr_t)whence);
        RETURN_OR_SET_ERRNO(ret);
}

int poll(struct pollfd *pollfds, nfds_t nfds, int timeout) {
        intptr_t ret = syscall3(NG_POLL, (intptr_t)pollfds,
                                          (intptr_t)nfds, (intptr_t)timeout);
        RETURN_OR_SET_ERRNO(ret);
}

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
        intptr_t ret =
            syscall6(NG_MMAP, (intptr_t)addr, (intptr_t)len, (intptr_t)prot,
                     (intptr_t)flags, (intptr_t)fd, (intptr_t)off);
        if (is_error(ret)) {
                errno = -ret;
                return (void *)-1;
        }
        return (void *)ret;
}

int munmap(void *addr, size_t len) {
        intptr_t ret =
            syscall2(NG_MUNMAP, (intptr_t)addr, (intptr_t)len);
        RETURN_OR_SET_ERRNO(ret);
}

int heapdbg(int type) {
        intptr_t ret = syscall1(NG_HEAPDBG, (intptr_t)type);
        RETURN_OR_SET_ERRNO(ret);
}

int setpgid(int pid, int pgid) {
        intptr_t ret = syscall2(NG_SETPGID, pid, pgid);
        RETURN_OR_SET_ERRNO(ret);
}

int top(int show_threads) {
        intptr_t ret = syscall1(NG_TOP, show_threads);
        RETURN_OR_SET_ERRNO(ret);
}

int clone(int (*fn)(void *), void *arg, void *new_stack, int flags) {
        intptr_t ret =
            syscall4(NG_CLONE0, (intptr_t)fn, (intptr_t)new_stack,
                     (intptr_t)arg, flags);
        RETURN_OR_SET_ERRNO(ret);
}

int load_module(int fd) {
        intptr_t ret = syscall1(NG_LOADMOD, fd);
        RETURN_OR_SET_ERRNO(ret);
}

noreturn int haltvm(int exit_code) {
        intptr_t ret = syscall1(NG_HALTVM, exit_code);
        __builtin_unreachable();
}

int openat(int fd, const char *name, int flags) {
        intptr_t ret =
            syscall3(NG_OPENAT, fd, (intptr_t)name, (intptr_t)flags);
        RETURN_OR_SET_ERRNO(ret);
}

int execveat(int fd, char *program, char **argv, char **envp) {
        intptr_t ret = syscall4(NG_EXECVEAT, fd, (intptr_t)program,
                                          (intptr_t)argv, (intptr_t)envp);
        RETURN_OR_SET_ERRNO(ret);
}

int ttyctl(int fd, int command, int arg) {
        intptr_t ret = syscall3(NG_TTYCTL, fd, command, arg);
        RETURN_OR_SET_ERRNO(ret);
}

int close(int fd) {
        intptr_t ret = syscall1(NG_CLOSE, fd);
        RETURN_OR_SET_ERRNO(ret);
}

int pipe(int pipefds[static 2]) {
        intptr_t ret = syscall1(NG_PIPE, (intptr_t)pipefds);
        RETURN_OR_SET_ERRNO(ret);
}

sighandler_t sigaction(int sig, sighandler_t handler, int flags) {
        intptr_t ret = syscall3(NG_SIGACTION, sig, (intptr_t)handler, flags);
        if (is_error(ret)) {
                errno = -ret;
                return NULL;
        }
        return (sighandler_t)ret;
}

int kill(pid_t pid, int sig) {
        intptr_t ret = syscall2(NG_KILL, pid, sig);
        RETURN_OR_SET_ERRNO(ret);
}

int sleepms(int ms) {
        intptr_t ret = syscall1(NG_SLEEPMS, ms);
        RETURN_OR_SET_ERRNO(ret);
}

ssize_t getdirents(int fd, struct ng_dirent *buf, ssize_t count) {
        intptr_t ret = syscall3(NG_GETDIRENTS, fd, (intptr_t)buf, count);
        RETURN_OR_SET_ERRNO(ret);
}

long ng_time() {
        intptr_t ret = syscall0(NG_TIME);
        RETURN_OR_SET_ERRNO(ret);
}

pid_t create(const char *executable) {
        intptr_t ret = syscall1(NG_CREATE, (intptr_t)executable);
        RETURN_OR_SET_ERRNO(ret);
}

int procstate(pid_t destination, enum procstate flags) {
        intptr_t ret = syscall2(NG_PROCSTATE, destination, flags);
        RETURN_OR_SET_ERRNO(ret);
}

int fault(enum fault_type type) {
        intptr_t ret = syscall1(NG_FAULT, type);
        RETURN_OR_SET_ERRNO(ret);
}

int sigprocmask(int op, const sigset_t *new, sigset_t *old) {
        intptr_t ret = syscall3(NG_SIGPROCMASK, op, (intptr_t)new, (intptr_t)old);
}

