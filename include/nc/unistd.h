
#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifndef _NG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

void debug_print(const char *message);

noreturn void exit(int status);
noreturn void exit_group(int status);

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

int setpgid(void);

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

off_t seek(int fd, off_t offset, int whence);
off_t lseek(int fd, off_t offset, int whence);

enum {
        STDIN_FILENO = 0,
        STDOUT_FILENO = 1,
        STDERR_FILENO = 2,
};

// extra stuff

int strace(bool enable);
int top(int show_threads);
int load_module(int fd);

#endif // !_NG

#endif // _UNISTD_H_

