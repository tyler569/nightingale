#pragma once
#ifndef IPSTACK_CORE_H
#define IPSTACK_CORE_H

#include <stdint.h>
#include <list.h>

struct net_device;
struct socket_impl;

struct pkb {
    struct net_device *from;
    list_node queue;
    int refcount;

    uint8_t user_anno[32];

    int length; // -1 if unknown
    char buffer[];
};

struct pkb *new_pk();
struct pkb *new_pk_len(size_t len);
void pk_incref(struct pkb *pk);
void pk_decref(struct pkb *pk);
void free_pk(struct pkb *pk);

typedef uint32_t be32;
typedef uint16_t be16;

struct __PACKED mac_address {
    char data[6];
};

struct ethernet_header;
struct arp_header;
struct ip_header;
struct udp_header;
struct tcp_header;
struct icmp_header;

#define ETH_MTU 1536

#endif // IPSTACK_CORE_H
