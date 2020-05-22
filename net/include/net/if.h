
#ifndef NET_NET_IF_H
#define NET_NET_IF_H

#include <basic.h>
#include <net/core.h>

typedef int ng_result_t;

struct net_driver_impl {
        ng_result_t (*send)(struct net_if *, struct pkb *);
};

struct arp_cache_line {
    be32 ip;
    struct mac_address mac;
};

struct arp_cache {
#define ARP_CACHE_LEN 32
    struct arp_cache_line cl[ARP_CACHE_LEN];
};

struct net_if {
    struct mac_address mac_address;
    be32 ip;
    be32 netmask;

    struct arp_cache arp_cache;
    list pending_mac_queries;

    struct net_driver_impl *drv;
};

#endif

