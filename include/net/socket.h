
#ifndef NIGHTINGALE_NET_SOCKET_H
#define NIGHTINGALE_NET_SOCKET_H

#include <ng/basic.h>
#include <fs/vfs.h>
#include <stddef.h>
#include <stdint.h>
#include "ip.h"

enum sock_type {
        SOCK_DGRAM,
};

enum af_type {
        AF_INET,
};

enum ipproto {
        SOCK_DEFAULT,
        SOCK_UDP,
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
        char sin_data[14];
};

typedef size_t socklen_t;

// void sockets_init(struct net_if* nic);

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport,
                   uint16_t othrport);

void socket_dispatch(struct ip_hdr *ip);

ssize_t socket_read(struct fs_node *sock_node, void *data, size_t len);
ssize_t socket_write(struct fs_node *sock_node, const void *data, size_t len);

/*
struct syscall_ret sys_socket(int domain, int type, int protocol); // ->
syscalls.h struct syscall_ret sys_bind0(int sockfd, uint32_t addr, size_t
addrlen); // -> syscalls.h struct syscall_ret sys_connect0(int sockfd, uint32_t
remote, uint16_t port); // -> syscalls.h
*/

#endif
