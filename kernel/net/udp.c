#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <stdio.h>
#include <string.h>

static uint16_t udp_checksum(
	struct pk *pk, struct sockaddr_in *src, struct sockaddr_in *dest) {
	struct udp_hdr *udp = L4(pk);
	uint16_t udp_len = ntohs(udp->length);

	// Create pseudo-header for checksum calculation
	struct udp_ip_psuedo_hdr pseudo = {
		.src = src->sin_addr,
		.dest = dest->sin_addr,
		.zero = 0,
		.protocol = IPPROTO_UDP,
		.length = udp->length,
	};

	uint16_t checksum = 0;
	checksum = net_checksum_add((uint16_t *)&pseudo, sizeof(pseudo), checksum);
	checksum = net_checksum_add((uint16_t *)udp, udp_len, checksum);

	return net_checksum_finish(checksum);
}

int udp_send(struct sockaddr_in *src, struct sockaddr_in *dest,
	const void *data, size_t data_len) {

	if (data_len > ETH_MTU - sizeof(struct eth_hdr) - sizeof(struct ip_hdr)
			- sizeof(struct udp_hdr)) {
		return -1; // Data too large
	}

	struct pk *pk = pk_alloc();
	if (!pk) {
		return -1;
	}

	// Set up packet offsets
	pk->l2_offset = 0;
	pk->l3_offset = sizeof(struct eth_hdr);
	pk->l4_offset = pk->l3_offset + sizeof(struct ip_hdr);

	// Fill UDP header
	struct udp_hdr *udp = L4(pk);
	udp->src_port = src->sin_port;
	udp->dest_port = dest->sin_port;
	udp->length = htons(sizeof(struct udp_hdr) + data_len);
	udp->checksum = 0; // Will be calculated later

	// Copy data
	uint8_t *payload = pk->data + pk->l4_offset + sizeof(struct udp_hdr);
	memcpy(payload, data, data_len);

	// Calculate checksum
	udp->checksum = udp_checksum(pk, src, dest);

	// Set packet length
	pk->len = pk->l4_offset + sizeof(struct udp_hdr) + data_len;

	// Fill IP header
	struct ip_hdr *ip = L3(pk);
	memset(ip, 0, sizeof(struct ip_hdr));
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->len = htons(sizeof(struct ip_hdr) + sizeof(struct udp_hdr) + data_len);
	ip->id = 0; // Should increment
	ip->flags = 0;
	ip->frag_offset = 0;
	ip->ttl = 64;
	ip->protocol = IPPROTO_UDP;
	ip->src = src->sin_addr;
	ip->dest = dest->sin_addr;
	ip->checksum = 0;
	ip->checksum = net_checksum((uint16_t *)ip, sizeof(struct ip_hdr));

	printf("UDP sending %zu bytes from port %d to port %d\n", data_len,
		ntohs(src->sin_port), ntohs(dest->sin_port));

	// Send via IP layer
	ip_egress(pk);

	return data_len;
}