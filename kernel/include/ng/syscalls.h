
#pragma once
#ifndef NG_SYSCALLS_H
#define NG_SYSCALLS_H

#include <basic.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/uname.h>
#include <ng/cpu.h>
#include <ng/fs.h>
#include <ng/net/socket.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

sysret sys_exit(int exit_status);
sysret sys_read(int fd, void *buf, size_t len);
sysret sys_write(int fd, void const *buf, size_t len);
sysret sys_fork(interrupt_frame *frame);
sysret sys_top(int show_threads);
sysret sys_getpid(void);
sysret sys_gettid(void);
sysret sys_execve(interrupt_frame *frame, char *file,
                char **argv, char **envp);
sysret sys_execveat(interrupt_frame *frame, int dir_fd, char *file,
                char **argv, char **envp);
sysret sys_wait4(pid_t);
sysret sys_socket(int, int, int);
sysret sys_strace(bool);
sysret sys_bind(int, struct sockaddr *, size_t);
sysret sys_connect(int, struct sockaddr *, size_t);
sysret sys_send(int fd, const void *buf, size_t len, int flags);
sysret sys_sendto(int fd, const void *buf, size_t len, int flags,
                              const struct sockaddr *, size_t);
sysret sys_recv(int fd, void *buf, size_t len, int flags);
sysret sys_recvfrom(int fd, void *buf, size_t len, int flags,
                                struct sockaddr *, size_t *);
sysret sys_waitpid(pid_t, int *, int);
sysret sys_dup2(int, int);
sysret sys_uname(struct utsname *);
sysret sys_yield(void);
sysret sys_seek(int fs, off_t offset, int whence);
sysret sys_poll(struct pollfd *, nfds_t, int);
sysret sys_mmap(void *, size_t, int, int, int, off_t);
sysret sys_munmap(void *, size_t);
sysret sys_heapdbg(int);
sysret sys_setpgid(void);
sysret sys_exit_group(int);
sysret sys_clone0(interrupt_frame *r, int (*fn)(void *), 
                              void *new_stack, void *arg, int flags);
sysret sys_loadmod(int fd);
sysret sys_haltvm(int exitst);

sysret sys_open(char *filename, int flags);
sysret sys_openat(int fd, char *filename, int flags);
sysret sys_close(int fd);
sysret sys_pipe(int pipefds[2]);
sysret sys_read(int fd, void *data, size_t len);
sysret sys_write(int fd, const void *data, size_t len);
sysret sys_dup2(int oldfd, int newfd);
sysret sys_seek(int fs, off_t offset, int whence);

sysret sys_ttyctl(int fd, int cmd, int arg);

sysret sys_sigaction(int signum, void *handler, int flags);
sysret sys_sigreturn(int code);
sysret sys_kill(pid_t pid, int sig);

#endif // NG_SYSCALLS_H

