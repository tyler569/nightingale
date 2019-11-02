
#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#define HEAPDBG_SUMMARY 2
#define HEAPDBG_DETAIL 1

// debug the kernel heap
int heapdbg(int type);

// debug the local heap
void print_pool();
void summarize_pool();

// halt qemu
noreturn void haltvm();

// ng-specific open functions
int ng_open(char *filename, int flags);
int ng_openat(int dir_fd, char *filename, int flags);

int ng_execve(char *program, char **argv, char **envp);
int ng_execveat(int fd, char *program, char **argv, char **envp);

int ttyctl(int fd, int command, int arg);

#endif // _NIGHTINGALE_H_

