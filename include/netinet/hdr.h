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
#define ETH_TYPE_IPV6 0x86DD

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

struct __PACKED ipv4_hdr {
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

struct __PACKED ipv6_hdr {
	uint32_t version : 4;
	uint32_t traffic_class : 8;
	uint32_t flow_label : 20;
	uint16_t payload_len;
	uint8_t next_header;
	uint8_t hop_limit;
	struct in6_addr src;
	struct in6_addr dest;
};

struct __PACKED icmp_hdr {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint32_t rest;
};

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8

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
