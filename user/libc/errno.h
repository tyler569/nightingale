
#pragma once
#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <ng_syscall.h>
#include <stdio.h>

// TODO: errno should be thread-local
extern int errno;

void perror(const char* const message);
const char* strerror(int errno);

#endif

