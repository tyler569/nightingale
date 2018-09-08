
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

int socket(int domain, int type, int protocol);
int connect0(int sock, uint32_t addr, size_t addrlen);
int bind0(int sock, uint32_t addr, uint16_t port);


#endif

