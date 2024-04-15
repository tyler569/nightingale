#include <net/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>

void ip_ingress(struct pk *pk);
void icmp_ingress(struct pk *pk);
void tcp_ingress(struct pk *pk);
void udp_ingress(struct pk *pk);

void ip_ingress(struct pk *pk) {
	struct ip_hdr *ip = (struct ip_hdr *)(pk->data + pk->l3_offset);
	ip->l4_offset = pk->l3_offset + (ip->ihl * 4);

	if (ip->proto == IP_PROTO_ICMP) {
		icmp_ingress(pk);
	} else if (ip->proto == IP_PROTO_TCP) {
		tcp_ingress(pk);
	} else if (ip->proto == IP_PROTO_UDP) {
		udp_ingress(pk);
	} else {
		pk_reject(pk);
	}
}

void icmp_ingress(struct pk *pk) {
	struct icmp_hdr *icmp = (struct icmp_hdr *)(pk->data + pk->l4_offset);

	if (icmp->type == ICMP_TYPE_ECHO_REQUEST) {
		icmp->type = ICMP_TYPE_ECHO_REPLY;
		pk->l3->ip.dst = pk->l3->ip.src;
		pk->l3->ip.src = pk->l3->ip.dst;
		pk->l3->ip.csum = 0;
		pk->l3->ip.csum = cksum((uint16_t *)pk->l3, pk->l3_offset);
		pk_send(pk);
	} else {
		pk_reject(pk);
	}
}

void tcp_ingress(struct pk *pk) { pk_reject(pk); }

void udp_ingress(struct pk *pk) {
	struct udp_hdr *udp = (struct udp_hdr *)(pk->data + pk->l4_offset);

	uint16_t port = ntohs(udp->dst_port);

	printf("udp_ingress: port=%d\n", port);

	pk_reject(pk);
}
