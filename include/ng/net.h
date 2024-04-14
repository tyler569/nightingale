#pragma once

#define ETH_MTU 1518

struct pk;
struct net_if;

struct net_if_vtbl {
	void (*send)(struct net_if *nif, struct pk *pk);
	struct pk *(*recv)(struct net_if *nif);
};

struct net_if {
	struct net_if_vtbl *vtbl;
};
