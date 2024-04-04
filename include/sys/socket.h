#pragma once

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

enum socket_mode {
    SOCKET_IDLE,
    SOCKET_BOUND,
    STREAM_LISTENING,
    STREAM_CONNECTED,
    STREAM_CLOSED,
};

enum socket_domain {
    AF_UNIX,
    // AF_INET,
};

enum socket_type {
    SOCK_DGRAM,
    SOCK_STREAM,
};

enum socket_protocol {
    PROTO_DEFAULT = 0,
    // PROTO_UDP,
    // PROTO_TCP,
};

typedef size_t socklen_t;
typedef int16_t sa_family_t;

// struct sockaddr_in {
//     sa_family_t sin_family;
//     uint16_t sin_port;
//     struct in_addr sin_addr;
//     char sin_zero[8];
// };

struct sockaddr {
    sa_family_t sin_family;
    char data[14];
};

#ifndef __kernel__

int socket(int domain, int type, int protocol);
int bind(int sockfd, struct sockaddr const *addr, socklen_t addrlen);

int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sock, struct sockaddr const *addr, socklen_t addrlen);

ssize_t send(int sock, void const *buf, size_t len, int flags);
ssize_t sendto(int sock, void const *buf, size_t len, int flags,
    struct sockaddr const *remote, socklen_t addrlen);
ssize_t recv(int sock, void *buf, size_t len, int flags);
ssize_t recvfrom(int sock, void *buf, size_t len, int flags,
    struct sockaddr *remote, socklen_t *addrlen);

#endif // ifndef __kernel__

END_DECLS

