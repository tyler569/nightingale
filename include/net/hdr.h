#pragma once

#include <netinet/in.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct __PACKED eth_hdr {
	struct eth_addr dest;
	struct eth_addr src;
	uint16_t type;
};

#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP 0x0800

struct __PACKED arp_hdr {
	uint16_t hw_type;
	uint16_t proto_type;
	uint8_t hw_addr_len;
	uint8_t proto_addr_len;
	uint16_t opcode;
	struct eth_addr src_hw_addr;
	struct in_addr src_proto_addr;
	struct eth_addr dest_hw_addr;
	struct in_addr dest_proto_addr;
};

#define ARP_REQUEST 1
#define ARP_REPLY 2

struct __PACKED ip4_hdr {
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

struct __PACKED udp_hdr {
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t length;
	uint16_t checksum;
};

struct __PACKED tcp_hdr {
	uint16_t src_port;
	uint16_t dest_port;
	uint32_t seq;
	uint32_t ack;
	uint8_t data_offset : 4;
	uint8_t reserved : 4;
	uint8_t flags;
	uint16_t window;
	uint16_t checksum;
	uint16_t urgent;
};

END_DECLS
