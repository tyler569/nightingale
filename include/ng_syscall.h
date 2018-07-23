
#pragma once
#ifndef _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_
#define _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_

#define SUCCESS 0
#define EINVAL 1
#define EWOULDBLOCK 2
#define EAGAIN EWOULDBLOCK
#define ENOEXEC 3
#define ENOENT 4

#define SYS_INVALID 0
#define SYS_DEBUGPRINT 1
#define SYS_EXIT 2
#define SYS_OPEN 3 // TODO
#define SYS_READ 4
#define SYS_WRITE 5
#define SYS_FORK 6
#define SYS_TOP 7
#define SYS_GETPID 8
#define SYS_GETTID 9
#define SYS_EXECVE 10

#endif

