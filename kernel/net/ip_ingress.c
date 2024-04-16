#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <rbtree.h>
#include <stdio.h>

void ip_ingress(struct pk *pk);
void icmp_ingress(struct pk *pk);
void tcp_ingress(struct pk *pk);
void udp_ingress(struct pk *pk);

void ip_ingress(struct pk *pk) {
	struct ipv4_hdr *ip = (struct ipv4_hdr *)(pk->data + pk->l3_offset);
	pk->l4_offset = pk->l3_offset + (ip->ihl * 4);

	if (ip->protocol == IPPROTO_ICMP) {
		icmp_ingress(pk);
	} else if (ip->protocol == IPPROTO_TCP) {
		tcp_ingress(pk);
	} else if (ip->protocol == IPPROTO_UDP) {
		udp_ingress(pk);
	} else {
		pk_reject(pk);
	}
}

void icmp_ingress(struct pk *pk) {
	struct icmp_hdr *icmp = (struct icmp_hdr *)(pk->data + pk->l4_offset);

	uint8_t type = icmp->type;
	switch (type) {
	case ICMP_ECHO_REQUEST:
		printf("icmp_ingress: type=ICMP_ECHO_REQUEST\n");
		break;
	case ICMP_ECHO_REPLY:
		printf("icmp_ingress: type=ICMP_ECHO_REPLY\n");
		break;
	default:
		printf("icmp_ingress: type=%d\n", type);
		break;
	}
}

void tcp_ingress(struct pk *pk) { pk_reject(pk); }

void udp_ingress(struct pk *pk) {
	struct udp_hdr *udp = (struct udp_hdr *)(pk->data + pk->l4_offset);

	uint16_t port = ntohs(udp->dest_port);

	printf("udp_ingress: port=%d\n", port);

	pk_reject(pk);
}
