#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/debug.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <stdio.h>
#include <string.h>

enum layer_type {
	ETHERNET,
	IPV4,
	IPV6,
	ARP,
	UDP,
	TCP,
};

void print_eth_addr(struct eth_addr *addr) {
	printf("%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[0], addr->addr[1],
		addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
}

void print_ip4_addr(struct in_addr *addr) {
	for (int i = 0; i < 4; i++) {
		printf("%d", addr->s_addr >> (i * 8) & 0xff);
		if (i < 3) {
			printf(".");
		}
	}
}

void print_ip6_addr(struct in6_addr *addr) {
	for (int i = 0; i < 16; i++) {
		printf("%02x", addr->s6_addr[i]);
		if (i % 2 == 1 && i < 15) {
			printf(":");
		}
	}
}

void print_ip6_addr_abbrev(struct in6_addr *addr) {
	int zero_start = -1;
	int zero_len = 0;

	for (int i = 0; i < 16; i += 2) {
		int j = 0;
		while (i + j < 16 && addr->s6_addr[i + j] == 0) {
			j++;
		}
		if (j > zero_len && j >= 2) {
			zero_start = i;
			zero_len = j;
		}
	}

	for (int i = 0; i < 16;) {
		if (i == zero_start) {
			printf("::");
			i += zero_len;
		} else {
			if (i % 2 == 0) {
				if (addr->s6_addr[i]) {
					printf("%x", addr->s6_addr[i]);
					printf("%02x", addr->s6_addr[i + 1]);
				} else {
					printf("%x", addr->s6_addr[i + 1]);
				}
				i += 2;
			} else {
				printf("%x", addr->s6_addr[i]);
				i++;
			}
			if (i < 15 && i != zero_start) {
				printf(":");
			}
		}
	}
}

void net_debug(enum layer_type type, void *data, size_t len) {
	printf("    ");
	switch (type) {
	case ETHERNET: {
		struct eth_hdr *hdr = data;
		printf("Ethernet: ");
		print_eth_addr(&hdr->src);
		printf(" -> ");
		print_eth_addr(&hdr->dest);
		printf("\n");
		switch (ntohs(hdr->type)) {
		case ETH_TYPE_ARP:
			net_debug(ARP, hdr + 1, len - sizeof(*hdr));
			break;
		case ETH_TYPE_IP:
			net_debug(IPV4, hdr + 1, len - sizeof(*hdr));
			break;
		case ETH_TYPE_IPV6:
			net_debug(IPV6, hdr + 1, len - sizeof(*hdr));
			break;
		default:
			printf("Unknown Ethernet type: %04x\n", ntohs(hdr->type));
		}
		break;
	}
	case IPV4: {
		struct ip_hdr *hdr = data;
		printf("IPv4: ");
		print_ip4_addr(&hdr->src);
		printf(" -> ");
		print_ip4_addr(&hdr->dest);
		printf("\n");
		switch (hdr->protocol) {
		case IPPROTO_UDP:
			net_debug(UDP, hdr + 1, ntohs(hdr->len) - sizeof(*hdr));
			break;
		case IPPROTO_TCP:
			net_debug(TCP, hdr + 1, ntohs(hdr->len) - sizeof(*hdr));
			break;
		default:
			printf("Unknown IPv4 protocol: %02x\n", hdr->protocol);
		}
		break;
	}
	case IPV6: {
		struct ipv6_hdr *hdr = data;
		printf("IPv6: ");
		print_ip6_addr_abbrev(&hdr->src);
		printf(" -> ");
		print_ip6_addr_abbrev(&hdr->dest);
		printf("\n");
		switch (hdr->next_header) {
		case IPPROTO_UDP:
			net_debug(UDP, hdr + 1, ntohs(hdr->payload_len));
			break;
		case IPPROTO_TCP:
			net_debug(TCP, hdr + 1, ntohs(hdr->payload_len));
			break;
		default:
			printf("Unknown IPv6 protocol: %02x\n", hdr->next_header);
		}
		break;
	}
	case ARP: {
		struct arp_hdr *hdr = data;
		printf("ARP: ");
		switch (ntohs(hdr->opcode)) {
		case ARP_REQUEST:
			printf("Who is ");
			print_ip4_addr(&hdr->dest_proto_addr);
			printf("? Tell ");
			print_ip4_addr(&hdr->src_proto_addr);
			printf("\n");
			break;
		case ARP_REPLY:
			print_ip4_addr(&hdr->src_proto_addr);
			printf(" is at ");
			print_eth_addr(&hdr->src_hw_addr);
			printf("\n");
			break;
		default:
			printf("Unknown ARP opcode: %04x\n", ntohs(hdr->opcode));
		}
		break;
	}
	case UDP: {
		struct udp_hdr *hdr = data;
		printf("UDP: %d -> %d\n", ntohs(hdr->src_port), ntohs(hdr->dest_port));
		hexdump(hdr + 1, len - sizeof(*hdr));
		break;
	}
	case TCP: {
		struct tcp_hdr *hdr = data;
		printf("TCP: %d -> %d\n", ntohs(hdr->src_port), ntohs(hdr->dest_port));
		hexdump(hdr + 1, len - sizeof(*hdr));
		break;
	}
	default:
		printf("Unknown layer type: %d\n", type);
	}
}

void net_debug_pk(struct pk *pk) { net_debug(ETHERNET, pk->data, pk->len); }

// static void net_ip6_print_test(struct in6_addr *addr) {
// 	printf("IPv6 address: ");
// 	print_ip6_addr(addr);
// 	printf("\n    (abbrev): ");
// 	print_ip6_addr_abbrev(addr);
// 	printf("\n");
// }

void net_test() { }

void net_debug_udp_echo(struct pk *pk) {
	struct pk *reply = pk_alloc();

	reply->len = pk->len;

	reply->l2_offset = pk->l2_offset;
	reply->l3_offset = pk->l3_offset;
	reply->l4_offset = pk->l4_offset;

	struct udp_hdr *pk_udp = L4(pk);
	struct udp_hdr *reply_udp = L4(reply);

	memcpy(reply->data + reply->l4_offset, pk->data + pk->l4_offset,
		pk->len - pk->l4_offset);

	reply_udp->src_port = pk_udp->dest_port;
	reply_udp->dest_port = pk_udp->src_port;
	reply_udp->length = pk_udp->length;
	reply_udp->checksum = 0;

	struct ip_hdr *pk_ip = L3(pk);
	struct ip_hdr *reply_ip = L3(reply);

	reply_ip->src = pk_ip->dest;
	reply_ip->dest = pk_ip->src;
	reply_ip->len = htons(reply->len - reply->l3_offset);
	reply_ip->id = pk_ip->id;
	reply_ip->ttl = 64;
	reply_ip->protocol = IPPROTO_UDP;
	reply_ip->ihl = sizeof(*reply_ip) / 4;
	reply_ip->version = 4;
	reply_ip->frag_offset = 0;
	reply_ip->flags = 0;
	reply_ip->checksum = 0;

	reply_ip->checksum = net_checksum((uint16_t *)reply_ip, sizeof(*reply_ip));

	struct udp_ip_psuedo_hdr pseudo = {
		.src = reply_ip->src,
		.dest = reply_ip->dest,
		.zero = 0,
		.protocol = IPPROTO_UDP,
		.length = reply_udp->length,
	};

	uint16_t csum = net_checksum_begin((uint16_t *)&pseudo, sizeof(pseudo));
	csum = net_checksum_add(
		(uint16_t *)reply_udp, ntohs(reply_udp->length), csum);
	reply_udp->checksum = net_checksum_finish(csum);

	struct eth_hdr *reply_eth = L2(reply);
	reply_eth->type = htons(ETH_TYPE_IP);

	ip_egress(reply);

	pk_free(reply);
}
