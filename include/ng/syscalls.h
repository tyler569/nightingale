
#pragma once
#ifndef NIGHTINGALE_SYSCALLS_H
#define NIGHTINGALE_SYSCALLS_H

#include <ng/basic.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/uname.h>
#include <arch/cpu.h>
#include <ng/fs.h>
#include <net/socket.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct syscall_ret sys_exit(int exit_status);
struct syscall_ret sys_read(int fd, void *buf, size_t len);
struct syscall_ret sys_write(int fd, void const *buf, size_t len);
struct syscall_ret sys_fork(interrupt_frame *frame);
struct syscall_ret sys_top(int show_threads);
struct syscall_ret sys_getpid(void);
struct syscall_ret sys_gettid(void);
struct syscall_ret sys_execve(interrupt_frame *frame, char *file, char **argv,
                              char **envp);
struct syscall_ret sys_wait4(pid_t);
struct syscall_ret sys_socket(int, int, int);
struct syscall_ret sys_strace(bool);
struct syscall_ret sys_bind(int, struct sockaddr *, size_t);
struct syscall_ret sys_connect(int, struct sockaddr *, size_t);
struct syscall_ret sys_send(int fd, const void *buf, size_t len, int flags);
struct syscall_ret sys_sendto(int fd, const void *buf, size_t len, int flags,
                              const struct sockaddr *, size_t);
struct syscall_ret sys_recv(int fd, void *buf, size_t len, int flags);
struct syscall_ret sys_recvfrom(int fd, void *buf, size_t len, int flags,
                                struct sockaddr *, size_t *);
struct syscall_ret sys_waitpid(pid_t, int *, int);
struct syscall_ret sys_dup2(int, int);
struct syscall_ret sys_uname(struct utsname *);
struct syscall_ret sys_yield(void);
struct syscall_ret sys_seek(int fs, off_t offset, int whence);
struct syscall_ret sys_poll(struct pollfd *, nfds_t, int);
struct syscall_ret sys_mmap(void *, size_t, int, int, int, off_t);
struct syscall_ret sys_munmap(void *, size_t);
struct syscall_ret sys_heapdbg(int);
struct syscall_ret sys_setpgid(void);
struct syscall_ret sys_exit_group(int);
struct syscall_ret sys_clone0(interrupt_frame *r, int (*fn)(void *), 
                              void *new_stack, void *arg, int flags);

#endif
