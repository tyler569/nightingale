#pragma once

#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>

#define ETH_MTU 1518

struct pk;
struct net_if;

struct net_if_vtbl {
	void (*send)(struct net_if *, struct pk *pk);
	// struct pk *(*recv)(struct net_if *nif);
};

struct arp_cache_entry {
	struct in_addr in;
	struct eth_addr eth;
	bool known;
	struct pk *parked;
};
static constexpr size_t ARP_CACHE_SIZE = 64;

struct net_if {
	struct net_if_vtbl *vtbl;
	struct eth_addr mac;
	struct arp_cache_entry cache[ARP_CACHE_SIZE];
	struct in_addr ip;
	struct in_addr netmask;
	struct in_addr gateway;
};

extern struct net_if *default_if;

#define NET_SEND(nif, pk) (nif)->vtbl->send(nif, pk)

uint16_t net_checksum_add(uint16_t *addr, int count, uint16_t sum);
uint16_t net_checksum_finish(uint16_t sum);
uint16_t net_checksum_begin(uint16_t *addr, int count);
uint16_t net_checksum(uint16_t *addr, int count);

void net_ingress(struct pk *pk);

void net_worker();

bool arp_cache_lookup(struct in_addr ip, struct eth_addr *mac);
void arp_ingress(struct pk *pk);

void ip_egress(struct pk *pk);
