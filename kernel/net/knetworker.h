
#ifndef NIGHTINGALE_NET_KNETWORKER_H
#define NIGHTINGALE_NET_KNETWORKER_H

#include <basic.h>
#include <queue.h>
#include "net_if.h"

struct queue incoming_packets;
struct queue knetworker_block;

struct pkt_desc {
    struct net_if* iface;
    size_t len;

    char data[];
};

void knetworker(void);

/*
 * void ip_incoming(struct ip_hdr* ip);
 * void arp_incoming(struct arp_pkt* arp);
 */

#endif

