#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/netfilter.h>
#include <ng/pk.h>
#include <stdio.h>

void ip_ingress(struct pk *pk);
void arp_ingress(struct pk *pk);

void net_ingress(struct pk *pk) {
	struct eth_hdr *eth = (struct eth_hdr *)pk->data;
	pk->l2_offset = 0;
	pk->l3_offset = sizeof(struct eth_hdr);

	uint16_t eth_type = ntohs(eth->type);
	printf("net_ingress: eth_type=%04x\n", eth_type);

	// PRE_ROUTING hook for IP packets
	if (eth_type == ETH_TYPE_IP) {
		enum nf_verdict verdict = nf_hook(NF_INET_PRE_ROUTING, pk);
		if (verdict == NF_DROP) {
			printf("Packet dropped by PRE_ROUTING filter\n");
			pk_drop(pk);
			return;
		}
	}

	switch (eth_type) {
	case ETH_TYPE_ARP:
		arp_ingress(pk);
		break;
	case ETH_TYPE_IP:
		ip_ingress(pk);
		break;
	default:
		pk_reject(pk);
		break;
	}
}