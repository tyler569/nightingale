
#pragma once
#ifndef NG_NET_NETWORK_H
#define NG_NET_NETWORK_H

#include <basic.h>
#include <ng/net/net_if.h>

void network_init(void);
void dispatch_packet(void *, size_t, struct net_if *);
void send_packet(struct net_if *nic, void *data, size_t len);

#endif // NG_NET_NETWORK_H

