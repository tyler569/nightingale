#pragma once

#include <netinet/in.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct ip4_hdr {
	uint8_t ihl : 4;
	uint8_t version : 4;
	uint8_t tos;
	uint16_t len;
	uint16_t id;
	uint16_t flags : 3;
	uint16_t frag_offset : 13;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	struct in_addr src;
	struct in_addr dest;
};

#define IP4_PROTOCOL_ICMP 1
#define IP4_PROTOCOL_TCP 6
#define IP4_PROTOCOL_UDP 17

END_DECLS
