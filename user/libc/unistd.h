
#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

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
int execve(char *program, char **argv, char **envp);
int dup2(int, int);

int setpgid(void);

// extra stuff

int strace(bool enable);
int top(int show_threads);

#endif
