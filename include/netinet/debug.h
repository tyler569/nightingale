#pragma once

#include <netinet/in.h>

void print_eth_addr(struct eth_addr *addr);
void print_ip4_addr(struct in_addr *addr);
void print_ip6_addr(struct in6_addr *addr);
void print_ip6_addr_abbrev(struct in6_addr *addr);

struct pk;
void net_debug_pk(struct pk *);