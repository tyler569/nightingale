#ifndef _AUTOGENERATED_kernel_include_ng_autogenerated_syscalls_h_
#define _AUTOGENERATED_kernel_include_ng_autogenerated_syscalls_h_

sysret sys_exit(int exit_code);
sysret sys_open(char *path, int flags, int mode);
sysret sys_read(int fd, void *data, size_t len);
sysret sys_write(int fd, const void *data, size_t len);
sysret sys_fork();
sysret sys_top(int show_threads);
sysret sys_getpid();
sysret sys_gettid();
sysret sys_execve(interrupt_frame *frame, char *program, char **argv,
                  char **envp);
sysret sys_strace(int trace);
sysret sys_waitpid(pid_t pid, int *exit_code, enum wait_options options);
sysret sys_dup2(int fd_dest, int fd_src);
sysret sys_uname(struct utsname *uname);
sysret sys_yield();
sysret sys_seek(int fd, off_t offset, int whence);
sysret sys_poll(struct pollfd *pollfd, nfds_t nfds, int timeout);
sysret sys_mmap(void *addr, size_t len, int prot, int flags, int fd,
                off_t offset);
sysret sys_munmap(void *addr, size_t len);
sysret sys_setpgid(pid_t pid, pid_t pgid);
sysret sys_exit_group(int exit_code);
sysret sys_clone0(interrupt_frame *frame, clone_fn *fn, void *arg,
                  void *new_stack, int flags);
sysret sys_loadmod(int fd);
sysret sys_haltvm(int exit_code);
sysret sys_openat(int fd, const char *name, int flags);
sysret sys_execveat(interrupt_frame *frame, int fd, char *program, char **argv,
                    char **envp);
sysret sys_ttyctl(int fd, int command, int arg);
sysret sys_close(int fd);
sysret sys_pipe(int *pipefds);
sysret sys_sigaction(int sig, sighandler_t handler, int flags);
sysret sys_sigreturn(int code);
sysret sys_kill(pid_t pid, int dig);
sysret sys_sleepms(int ms);
sysret sys_readdir(int fd, struct ng_dirent *buf, size_t count);
sysret sys_xtime();
sysret sys_create(const char *executable);
sysret sys_procstate(pid_t pid, enum procstate flags);
sysret sys_fault(enum fault_type fault);
sysret sys_trace(enum trace_command cmd, pid_t pid, void *addr, void *data);
sysret sys_sigprocmask(int op, const sigset_t *new, sigset_t *old);
sysret sys_syscall_test(char *buffer);

#endif
