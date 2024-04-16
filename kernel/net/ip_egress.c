#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>

struct ip_output {
	struct net_if *interface;
	in_addr_t src_ip;
};

struct ip_output outputs[8];

void ip_egress(struct pk *pk) {
	struct ipv4_hdr *ip = (struct ipv4_hdr *)(pk->data + pk->l3_offset);
	in_addr_t src_ip = ntohl(ip->src.s_addr);

	for (int i = 0; i < 8; i++) {
		if (outputs[i].interface == nullptr) {
			break;
		}

		if (outputs[i].src_ip == src_ip) {
			struct net_if *net_if = outputs[i].interface;
			NET_SEND(net_if, pk);
			return;
		}
	}
}