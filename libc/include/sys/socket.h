#pragma once
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <stddef.h>
#include <stdint.h>

enum sock_type {
    SOCK_DGRAM,
    SOCK_STREAM,
};

enum af_type {
    AF_INET,
    AF_UNIX,
};

enum ipproto {
    IPPROTO_UDP = 17,
};

struct in_addr {
    uint32_t s_addr;
};

struct sockaddr_in {
    int16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

struct sockaddr {
    int16_t sin_family;
    char data[14];
};

typedef size_t socklen_t;

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
