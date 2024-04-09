#pragma once

#include <netinet/in.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct eth_hdr {
	struct eth_addr dest;
	struct eth_addr src;
	uint16_t type;
};

#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP 0x0800

END_DECLS
