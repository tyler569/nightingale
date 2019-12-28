
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

uint64_t flow_hash(uint32_t myip, uint32_t otherip, uint16_t myport,
                   uint16_t otherport);

void socket_dispatch(struct ip_hdr *ip);

ssize_t socket_read(struct open_file *, void *data, size_t len);
ssize_t socket_write(struct open_file *, const void *data, size_t len);

#endif // NG_NET_SOCKET_H

