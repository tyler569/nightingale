
#pragma once
#ifndef NG_NET_NETWORK_H
#define NG_NET_NETWORK_H

#include <basic.h>
#include <ng/net/net_if.h>

void network_init(void);
void dispatch_packet(void *, size_t, struct net_if *);

#endif // NG_NET_NETWORK_H

