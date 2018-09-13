
#pragma once
#ifndef _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_
#define _NIGHTINGALE_EXTERNAL_SYSCALL_INTERFACE_H_

#define SUCCESS 0
#define EINVAL 1
#define EWOULDBLOCK 2
#define EAGAIN EWOULDBLOCK
#define ENOEXEC 3
#define ENOENT 4
#define EAFNOSUPPORT 5
#define ECHILD 6
#define EPERM 7

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
#define SYS_WAIT4 11
#define SYS_SOCKET 12
#define SYS_BIND0 13
#define SYS_CONNECT0 14
#define SYS_STRACE 15
#define SYS_BIND 16
#define SYS_CONNECT 17
#define SYS_SEND 18
#define SYS_SENDTO 19
#define SYS_RECV 20
#define SYS_RECVFROM 21
#define SYS_WAITPID 22
#define SYS_DUP2 23

#define SYSCALL_MAX 23

/* waitpid flags */
#define WNOHANG 1


#endif

