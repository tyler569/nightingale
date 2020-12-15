noreturn void _exit(int exit_code) {
    intptr_t ret = syscall1(NG__EXIT, (intptr_t)exit_code);
    __builtin_unreachable();
}
int open(char *path, int flags, int mode) {
    intptr_t ret = syscall3(NG_OPEN, (intptr_t)path, (intptr_t)flags, (intptr_t)mode);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
ssize_t read(int fd, void *data, size_t len) {
    intptr_t ret = syscall3(NG_READ, (intptr_t)fd, (intptr_t)data, (intptr_t)len);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
ssize_t write(int fd, const void *data, size_t len) {
    intptr_t ret = syscall3(NG_WRITE, (intptr_t)fd, (intptr_t)data, (intptr_t)len);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
pid_t fork() {
    intptr_t ret = syscall0(NG_FORK);
    if (is_error(ret)) {
        errno = -ret;
        return (pid_t)-1;
    } else {
        return (pid_t)ret;
    }

}
int top(int show_threads) {
    intptr_t ret = syscall1(NG_TOP, (intptr_t)show_threads);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
pid_t getpid() {
    intptr_t ret = syscall0(NG_GETPID);
    if (is_error(ret)) {
        errno = -ret;
        return (pid_t)-1;
    } else {
        return (pid_t)ret;
    }

}
pid_t gettid() {
    intptr_t ret = syscall0(NG_GETTID);
    if (is_error(ret)) {
        errno = -ret;
        return (pid_t)-1;
    } else {
        return (pid_t)ret;
    }

}
int execve(char *program, char *const *argv, char *const *envp) {
    intptr_t ret = syscall3(NG_EXECVE, (intptr_t)program, (intptr_t)argv, (intptr_t)envp);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int socket(int domain, int type, int protocol) {
    intptr_t ret = syscall3(NG_SOCKET, (intptr_t)domain, (intptr_t)type, (intptr_t)protocol);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int strace(int trace) {
    intptr_t ret = syscall1(NG_STRACE, (intptr_t)trace);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int bind(int fd, const struct sockaddr *addr, socklen_t addr_len) {
    intptr_t ret = syscall3(NG_BIND, (intptr_t)fd, (intptr_t)addr, (intptr_t)addr_len);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int connect(int sock, const struct sockaddr *addr, socklen_t addr_len) {
    intptr_t ret = syscall3(NG_CONNECT, (intptr_t)sock, (intptr_t)addr, (intptr_t)addr_len);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
ssize_t send(int sock, const void *buf, size_t len, int flags) {
    intptr_t ret = syscall4(NG_SEND, (intptr_t)sock, (intptr_t)buf, (intptr_t)len, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
ssize_t sendto(int sock, const void *buf, size_t len, int flags, const struct sockaddr *remote, socklen_t addr_len) {
    intptr_t ret = syscall6(NG_SENDTO, (intptr_t)sock, (intptr_t)buf, (intptr_t)len, (intptr_t)flags, (intptr_t)remote, (intptr_t)addr_len);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
ssize_t recv(int sock, void *buf, size_t len, int flags) {
    intptr_t ret = syscall4(NG_RECV, (intptr_t)sock, (intptr_t)buf, (intptr_t)len, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
ssize_t recvfrom(int sock, void *buf, size_t len, int flags, struct sockaddr *remote, socklen_t *addr_len) {
    intptr_t ret = syscall6(NG_RECVFROM, (intptr_t)sock, (intptr_t)buf, (intptr_t)len, (intptr_t)flags, (intptr_t)remote, (intptr_t)addr_len);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
int waitpid(pid_t pid, int *exit_code, enum wait_options options) {
    intptr_t ret = syscall3(NG_WAITPID, (intptr_t)pid, (intptr_t)exit_code, (intptr_t)options);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int dup2(int fd_dest, int fd_src) {
    intptr_t ret = syscall2(NG_DUP2, (intptr_t)fd_dest, (intptr_t)fd_src);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int uname(struct utsname *uname) {
    intptr_t ret = syscall1(NG_UNAME, (intptr_t)uname);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int yield() {
    intptr_t ret = syscall0(NG_YIELD);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
off_t seek(int fd, off_t offset, int whence) {
    intptr_t ret = syscall3(NG_SEEK, (intptr_t)fd, (intptr_t)offset, (intptr_t)whence);
    if (is_error(ret)) {
        errno = -ret;
        return (off_t)-1;
    } else {
        return (off_t)ret;
    }

}
int poll(struct pollfd *pollfd, nfds_t nfds, int timeout) {
    intptr_t ret = syscall3(NG_POLL, (intptr_t)pollfd, (intptr_t)nfds, (intptr_t)timeout);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
void* mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
    intptr_t ret = syscall6(NG_MMAP, (intptr_t)addr, (intptr_t)len, (intptr_t)prot, (intptr_t)flags, (intptr_t)fd, (intptr_t)offset);
    if (is_error(ret)) {
        errno = -ret;
        return (void*)-1;
    } else {
        return (void*)ret;
    }

}
int munmap(void *addr, size_t len) {
    intptr_t ret = syscall2(NG_MUNMAP, (intptr_t)addr, (intptr_t)len);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int setpgid(pid_t pid, pid_t pgid) {
    intptr_t ret = syscall2(NG_SETPGID, (intptr_t)pid, (intptr_t)pgid);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
noreturn void exit_group(int exit_code) {
    intptr_t ret = syscall1(NG_EXIT_GROUP, (intptr_t)exit_code);
    __builtin_unreachable();
}
int clone0(clone_fn *fn, void *arg, void *new_stack, int flags) {
    intptr_t ret = syscall4(NG_CLONE0, (intptr_t)fn, (intptr_t)arg, (intptr_t)new_stack, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int loadmod(int fd) {
    intptr_t ret = syscall1(NG_LOADMOD, (intptr_t)fd);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
noreturn int haltvm(int exit_code) {
    intptr_t ret = syscall1(NG_HALTVM, (intptr_t)exit_code);
    __builtin_unreachable();
}
int openat(int fd, const char *name, int flags) {
    intptr_t ret = syscall3(NG_OPENAT, (intptr_t)fd, (intptr_t)name, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int execveat(int fd, char *program, char *const *argv, char *const *envp) {
    intptr_t ret = syscall4(NG_EXECVEAT, (intptr_t)fd, (intptr_t)program, (intptr_t)argv, (intptr_t)envp);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int ttyctl(int fd, int command, int arg) {
    intptr_t ret = syscall3(NG_TTYCTL, (intptr_t)fd, (intptr_t)command, (intptr_t)arg);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int close(int fd) {
    intptr_t ret = syscall1(NG_CLOSE, (intptr_t)fd);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int pipe(int *pipefds) {
    intptr_t ret = syscall1(NG_PIPE, (intptr_t)pipefds);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
sighandler_t sigaction(int sig, sighandler_t handler, int flags) {
    intptr_t ret = syscall3(NG_SIGACTION, (intptr_t)sig, (intptr_t)handler, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (sighandler_t)-1;
    } else {
        return (sighandler_t)ret;
    }

}
noreturn int sigreturn(int code) {
    intptr_t ret = syscall1(NG_SIGRETURN, (intptr_t)code);
    __builtin_unreachable();
}
int kill(pid_t pid, int dig) {
    intptr_t ret = syscall2(NG_KILL, (intptr_t)pid, (intptr_t)dig);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int sleepms(int ms) {
    intptr_t ret = syscall1(NG_SLEEPMS, (intptr_t)ms);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
ssize_t readdir(int fd, struct ng_dirent *buf, size_t count) {
    intptr_t ret = syscall3(NG_READDIR, (intptr_t)fd, (intptr_t)buf, (intptr_t)count);
    if (is_error(ret)) {
        errno = -ret;
        return (ssize_t)-1;
    } else {
        return (ssize_t)ret;
    }

}
long xtime() {
    intptr_t ret = syscall0(NG_XTIME);
    if (is_error(ret)) {
        errno = -ret;
        return (long)-1;
    } else {
        return (long)ret;
    }

}
pid_t create(const char *executable) {
    intptr_t ret = syscall1(NG_CREATE, (intptr_t)executable);
    if (is_error(ret)) {
        errno = -ret;
        return (pid_t)-1;
    } else {
        return (pid_t)ret;
    }

}
int procstate(pid_t pid, enum procstate flags) {
    intptr_t ret = syscall2(NG_PROCSTATE, (intptr_t)pid, (intptr_t)flags);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int fault(enum fault_type fault) {
    intptr_t ret = syscall1(NG_FAULT, (intptr_t)fault);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int trace(enum trace_command cmd, pid_t pid, void *addr, void *data) {
    intptr_t ret = syscall4(NG_TRACE, (intptr_t)cmd, (intptr_t)pid, (intptr_t)addr, (intptr_t)data);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int sigprocmask(int op, const sigset_t *new, sigset_t *old) {
    intptr_t ret = syscall3(NG_SIGPROCMASK, (intptr_t)op, (intptr_t)new, (intptr_t)old);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int unlink(const char *pathname) {
    intptr_t ret = syscall1(NG_UNLINK, (intptr_t)pathname);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int chmod(const char *path, mode_t mode) {
    intptr_t ret = syscall2(NG_CHMOD, (intptr_t)path, (intptr_t)mode);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int fchmod(int fd, mode_t mode) {
    intptr_t ret = syscall2(NG_FCHMOD, (intptr_t)fd, (intptr_t)mode);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int listen(int fd, int backlog) {
    intptr_t ret = syscall2(NG_LISTEN, (intptr_t)fd, (intptr_t)backlog);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int accept(int fd, struct sockaddr *addr, socklen_t *addr_len) {
    intptr_t ret = syscall3(NG_ACCEPT, (intptr_t)fd, (intptr_t)addr, (intptr_t)addr_len);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
int syscall_test(char *buffer) {
    intptr_t ret = syscall1(NG_SYSCALL_TEST, (intptr_t)buffer);
    if (is_error(ret)) {
        errno = -ret;
        return (int)-1;
    } else {
        return (int)ret;
    }

}
