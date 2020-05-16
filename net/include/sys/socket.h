
#pragma once
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <stddef.h>
#include <stdint.h>
#include <netinet/in.h>

enum sock_type {
        SOCK_DGRAM,
        SOCK_STREAM,
        SOCK_SEQPACKET,
};

enum socket_options {
        SO_ACCEPTCON,
        SO_BROADCAST,
        SO_DEBUG,
        SO_ERROR,
        SO_KEEPALIVE,
        SO_LINGER,
        SO_OOBINLINE,
        SO_RCVBUF,
        SO_RCVLOWAT,
        SO_RCVTIMEO,
        SO_REUSEADDR,
        SO_SNDBUF,
        SO_SNDLOWAT,
        SO_SNDTIMEO,
        SO_TYPE,
};

enum af_type {
        AF_INET,
        AF_UNIX,
};

#ifndef _NG

int socket(int domain, int type, int protocol);

int bind(int sockfd, struct sockaddr const *addr, socklen_t addrlen);
int connect(int sock, struct sockaddr const *addr, socklen_t addrlen);

ssize_t send(int sock, void const *buf, size_t len, int flags);
ssize_t sendto(int sock, void const *buf, size_t len, int flags,
               struct sockaddr const *remote, socklen_t addrlen);

ssize_t recv(int sock, void *buf, size_t len, int flags);
ssize_t recvfrom(int sock, void *buf, size_t len, int flags,
                 struct sockaddr *remote, socklen_t *addrlen);

#endif // _NG

#endif // _SYS_SOCKET_H_

