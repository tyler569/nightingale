
#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stddef.h>
#include <stdint.h>

extern int errno; // TODO: errno.h

void debug_print(const char *message);

__attribute__((noreturn)) void exit(int status);

ssize_t read(int fd, void *data, size_t len);
ssize_t write(int fd, const void *data, size_t len);
pid_t fork(void);
void top();

#endif

