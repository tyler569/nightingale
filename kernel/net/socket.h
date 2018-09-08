
#ifndef NIGHTINGALE_NET_SOCKET_H
#define NIGHTINGALE_NET_SOCKET_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>

#include <fs/vfs.h>
#include "net_if.h"

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

void sockets_init(struct net_if* nic);

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport, uint16_t othrport);

size_t socket_read(struct fs_node* sock_node, void* data, size_t len);
size_t socket_write(struct fs_node* sock_node, const void* data, size_t len);

/*
struct syscall_ret sys_socket(int domain, int type, int protocol); // -> syscalls.h
struct syscall_ret sys_bind0(int sockfd, uint32_t addr, size_t addrlen); // -> syscalls.h
struct syscall_ret sys_connect0(int sockfd, uint32_t remote, uint16_t port); // -> syscalls.h
*/

#endif

