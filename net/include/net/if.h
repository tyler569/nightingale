
#ifndef NET_NET_IF_H
#define NET_NET_IF_H

#include <basic.h>
#include <net/core.h>

struct net_drv_impl {
        ng_result (*send)(struct net_device *, struct pkb *);
};

struct arp_cache_line {
    be32 ip;
    struct mac_address mac;
};

struct arp_cache {
#define ARP_CACHE_LEN 32
    struct arp_cache_line cl[ARP_CACHE_LEN];
};

enum net_device_type {
        LOOPBACK,
        RTL8139,
};

struct net_device {
    struct mac_address mac_address;
    be32 ip;
    be32 netmask;

    struct arp_cache arp_cache;
    list pending_mac_queries;

    enum net_device_type type;
    struct net_drv_impl *drv;
    void *device_impl;
};

#endif

