#include <arpa/inet.h>
#include <netinet/debug.h>
#include <netinet/hdr.h>
#include <netinet/in.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <stdio.h>
#include <string.h>

struct arp_entry {
	struct eth_addr mac;
	struct in_addr ip;
};

static struct in_addr my_ip = { .s_addr = 0x0f02000a };
static struct eth_addr my_mac = { { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 } };
static struct eth_addr broadcast_mac
	= { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };

static struct arp_entry arp_cache[256];

void arp_cache_add(struct eth_addr mac, struct in_addr ip) {
	for (int i = 0; i < 256; i++) {
		if (arp_cache[i].ip.s_addr == 0) {
			arp_cache[i].mac = mac;
			arp_cache[i].ip = ip;
			return;
		}
	}
}

bool arp_cache_lookup(struct in_addr ip, struct eth_addr *mac) {
	for (int i = 0; i < 256; i++) {
		if (arp_cache[i].ip.s_addr == ip.s_addr) {
			*mac = arp_cache[i].mac;
			return true;
		}
	}
	return false;
}

void arp_cache_clear() {
	for (int i = 0; i < 256; i++) {
		arp_cache[i].ip.s_addr = 0;
	}
}

void arp_cache_print() {
	printf("ARP cache:\n");
	for (int i = 0; i < 256; i++) {
		if (arp_cache[i].ip.s_addr != 0) {
			printf("  ");
			print_ip4_addr(&arp_cache[i].ip);
			printf(" -> ");
			print_eth_addr(&arp_cache[i].mac);
			printf("\n");
		}
	}
}

void arp_ingress(struct pk *pk) {
	struct eth_hdr *eth = (struct eth_hdr *)(pk->data + pk->l2_offset);
	struct arp_hdr *arp = (struct arp_hdr *)(pk->data + pk->l3_offset);

	if (arp->proto_addr_len != 4 || ntohs(arp->proto_type) != ETH_TYPE_IP) {
		pk_drop(pk);
		return;
	}

	struct eth_addr eth_src;
	if (!arp_cache_lookup(arp->src_proto_addr, &eth_src)) {
		arp_cache_add(eth->src, arp->src_proto_addr);
	}

	if (ntohs(arp->opcode) != ARP_REQUEST) {
		pk_done(pk);
	}

	if (arp->dest_proto_addr.s_addr != my_ip.s_addr) {
		pk_done(pk);
		return;
	}

	// TODO GARBAGE
	void ip_neighbor_add(struct in_addr ip, struct net_if * nif);
	ip_neighbor_add(arp->dest_proto_addr, pk->origin_if);

	struct pk *reply = pk_alloc();
	reply->l2_offset = pk->l2_offset;
	reply->l3_offset = pk->l3_offset;

	struct eth_hdr *reply_eth
		= (struct eth_hdr *)(reply->data + reply->l2_offset);
	struct arp_hdr *reply_arp
		= (struct arp_hdr *)(reply->data + reply->l3_offset);

	reply_eth->dest = eth->src;
	reply_eth->src = my_mac;
	reply_eth->type = htons(ETH_TYPE_ARP);

	reply_arp->hw_type = htons(1);
	reply_arp->proto_type = htons(ETH_TYPE_IP);
	reply_arp->hw_addr_len = 6;
	reply_arp->proto_addr_len = 4;
	reply_arp->opcode = htons(ARP_REPLY);
	reply_arp->src_hw_addr = my_mac;
	reply_arp->src_proto_addr = my_ip;
	reply_arp->dest_hw_addr = eth->src;
	reply_arp->dest_proto_addr = arp->src_proto_addr;

	reply->len = reply->l3_offset + sizeof(struct arp_hdr);

	printf("sending:\n");
	net_debug_pk(reply);

	NET_SEND(pk->origin_if, reply);

	pk_done(pk);
}
