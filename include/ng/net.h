#pragma once

#include <netinet/in.h>
#include <stdint.h>

#define ETH_MTU 1518

struct pk;
struct net_if;

struct net_if_vtbl {
	void (*send)(struct net_if *, struct pk *pk);
	// struct pk *(*recv)(struct net_if *nif);
};

struct net_if {
	struct net_if_vtbl *vtbl;
};

uint16_t net_checksum_add(uint16_t *addr, int count, uint16_t sum);
uint16_t net_checksum_finish(uint16_t sum);
uint16_t net_checksum_begin(uint16_t *addr, int count);
uint16_t net_checksum(uint16_t *addr, int count);

void print_ip4_addr(struct in_addr *);
void print_ip6_addr(struct in6_addr *);
void print_ip6_addr_abbrev(struct in6_addr *);
