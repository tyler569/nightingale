
#ifndef NET_NETINET_IN_H
#define NET_NETINET_IN_H

#include <inttypes.h>

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

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

enum ipproto {
        IPPROTO_ICMP = 1,
        IPPROTO_TCP = 6,
        IPPROTO_UDP = 17,
};

typedef size_t socklen_t;

#endif
