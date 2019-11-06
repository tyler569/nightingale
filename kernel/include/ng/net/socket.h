
#pragma once
#ifndef NG_NET_SOCKET_H
#define NG_NET_SOCKET_H

#include <basic.h>
#include <nc/sys/socket.h>
#include <ng/fs.h>
#include <stddef.h>
#include <stdint.h>
#include "ip.h"

void sockets_init();

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport,
                   uint16_t othrport);

void socket_dispatch(struct ip_hdr *ip);

ssize_t socket_read(struct open_fd *, void *data, size_t len);
ssize_t socket_write(struct open_fd *, const void *data, size_t len);

/*
struct syscall_ret sys_socket(int domain, int type, int protocol); // -> syscalls.h
struct syscall_ret sys_bind0(int sockfd, uint32_t addr, size_t addrlen); // -> syscalls.h
struct syscall_ret sys_connect0(int sockfd, uint32_tremote, uint16_t port); // -> syscalls.h
*/

#endif // NG_NET_SOCKET_H

