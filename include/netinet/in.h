#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};

struct in6_addr {
	uint8_t s6_addr[16];
};

struct eth_addr {
	uint8_t addr[6];
};

struct sockaddr {
	uint16_t sa_family;
	uint8_t sa_data[14];
};

struct sockaddr_in {
	uint16_t sin_family;
	in_port_t sin_port;
	in_addr_t sin_addr;
	uint8_t sin_zero[8];
};

struct sockaddr_un {
	uint16_t sun_family;
	uint8_t sun_path[108];
};

struct sockaddr_in6 {
	uint16_t sin6_family;
	uint16_t sin6_port;
	uint32_t sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t sin6_scope_id;
};

enum {
	IPPROTO_IP = 0,
	IPPROTO_ICMP = 1,
	IPPROTO_TCP = 6,
	IPPROTO_UDP = 17,
	IPPRTOTO_IPV6 = 41,
	IPPROTO_RAW = 255,
};

enum {
	INADDR_ANY = 0,
	INADDR_LOOPBACK = 0x7f000001,
	INADDR_BROADCAST = 0xffffffff,

	INET_ADDRSTRLEN = 16,
	INET6_ADDRSTRLEN = 46,
};

END_DECLS
