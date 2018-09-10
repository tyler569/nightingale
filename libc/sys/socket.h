
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdint.h>
#include <stddef.h>

enum {
    SOCK_DGRAM = 0,

    AF_INET = 0,

    SOCK_UDP = 1,
    PROTO_UDP = 17,
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

struct sockaddr { // sockaddr_in *is* a sockaddr
    int16_t sin_family;
    char data[14];
};

typedef size_t socklen_t;

int socket(int domain, int type, int protocol);
int connect0(int sock, uint32_t addr, size_t addrlen);
int bind0(int sock, uint32_t addr, uint16_t port);

int connect(int sock, struct sockaddr const* addr, socklen_t addrlen);
int bind(int sockfd, struct sockaddr const* addr, socklen_t addrlen);

ssize_t send(int sock, void const* buf, size_t len, int flags);
ssize_t sendto(int sock, void const* buf, size_t len, int flags,
               struct sockaddr const* remote, socklen_t addrlen);

ssize_t recv(int sock, void* buf, size_t len, int flags);
ssize_t recvfrom(int sock, void* buf, size_t len, int flags,
               struct sockaddr* remote, socklen_t* addrlen);


#endif

