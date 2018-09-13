
#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


void debug_print(const char *message);

__attribute__((noreturn)) void exit(int status);

size_t read(int fd, void *data, size_t len);
size_t write(int fd, const void *data, size_t len);
pid_t fork(void);
void top(void);
pid_t getpid(void);
pid_t gettid(void);
void execve(char *program, char **argv, char **envp);
int dup2(int, int);

// extra stuff

int strace(bool enable);

#endif

