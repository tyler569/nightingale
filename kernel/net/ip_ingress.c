#include <arpa/inet.h>
#include <netinet/debug.h>
#include <netinet/hdr.h>
#include <ng/fs/inet_socket.h>
#include <ng/net.h>
#include <ng/netfilter.h>
#include <ng/pk.h>
#include <rbtree.h>
#include <stdio.h>
#include <string.h>

void ip_ingress(struct pk *pk);
void icmp_ingress(struct pk *pk);
void tcp_ingress(struct pk *pk);
void udp_ingress(struct pk *pk);

void ip_ingress(struct pk *pk) {
	struct ip_hdr *ip = (struct ip_hdr *)(pk->data + pk->l3_offset);
	pk->l4_offset = pk->l3_offset + (ip->ihl * 4);

	// LOCAL_IN hook - packet is destined for this host
	enum nf_verdict verdict = nf_hook(NF_INET_LOCAL_IN, pk);
	if (verdict == NF_DROP) {
		printf("Packet dropped by LOCAL_IN filter\n");
		pk_drop(pk);
		return;
	}

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

void tcp_ingress(struct pk *pk) {
	printf("tcp_ingress\n");

	pk_drop(pk);
}

void net_debug_udp_echo(struct pk *pk);

void udp_ingress(struct pk *pk) {
	struct udp_hdr *udp = L4(pk);

	uint16_t dest_port = ntohs(udp->dest_port);
	uint16_t src_port = ntohs(udp->src_port);

	printf("udp_ingress: dest_port=%d src_port=%d\n", dest_port, src_port);

	// Find socket bound to this port
	struct vnode *socket = find_socket_by_port(dest_port);
	if (socket) {
		printf("Delivering UDP packet to socket on port %d\n", dest_port);
		inet_socket_deliver(socket, pk);
	} else {
		printf("No socket bound to port %d, dropping packet\n", dest_port);
		pk_drop(pk);
	}
}
