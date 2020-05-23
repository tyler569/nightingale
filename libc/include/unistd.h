
#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <basic.h>

#ifndef _NG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

void debug_print(const char *message);

ssize_t read(int fd, void *data, size_t len);
ssize_t write(int fd, const void *data, size_t len);
pid_t fork(void);
pid_t clone(int (*fn)(void *), void *arg, void *new_stack, int flags);
pid_t getpid(void);
pid_t gettid(void);
int execve(const char *program, char *const *argv, char *const *envp);
int execvp(const char *program, char *const *argv);
int dup2(int, int);
int isatty(int fd);

int setpgid(int pid, int pgid);

off_t seek(int fd, off_t offset, int whence);
off_t lseek(int fd, off_t offset, int whence);

enum std_filenos {
        STDIN_FILENO = 0,
        STDOUT_FILENO = 1,
        STDERR_FILENO = 2,
};

// extra stuff

int strace(bool enable);
int top(int show_threads);
int load_module(int fd);

int sleep(int seconds);
int sleepms(int milliseconds);

#ifdef __cplusplus
int pipe(int pipefds[]);
#else
int pipe(int pipefds[static 2]);
#endif // __cplusplus

#endif // !_NG

#endif // _UNISTD_H_
