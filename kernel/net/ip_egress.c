#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>

struct ip_neighbor {
	struct in_addr src_ip;
	struct net_if *nif;
};

#define NUM_IP_NEIGHBORS 10

struct ip_neighbor ip_neighbors[NUM_IP_NEIGHBORS];

struct ip_neighbor *find_ip_neighbor(struct in_addr *ip) {
	for (int i = 0; i < NUM_IP_NEIGHBORS; i++) {
		if (ip_neighbors[i].src_ip.s_addr == ip->s_addr) {
			return &ip_neighbors[i];
		}
	}
	return nullptr;
}

struct ip_neighbor *find_ip_neighbor_for_pk(struct pk *pk) {
	struct ip_hdr *ip_hdr = L3(pk);
	return find_ip_neighbor(&ip_hdr->src);
}

void ip_egress(struct pk *pk) {
	// find the next hop
	struct ip_neighbor *neighbor = find_ip_neighbor_for_pk(pk);

	// if the next hop is not found, drop the packet
	if (!neighbor) {
		pk_drop(pk);
		return;
	}

	// decrement the TTL
	struct ip_hdr *ip_hdr = L3(pk);
	ip_hdr->ttl--;

	// recompute the checksum
	ip_hdr->checksum = 0;
	ip_hdr->checksum
		= htons(net_checksum((uint16_t *)ip_hdr, sizeof(struct ip_hdr)));

	// set the source MAC address
	struct eth_hdr *eth_hdr = L2(pk);
	eth_hdr->src = neighbor->nif->mac;

	// set the destination MAC address
	struct eth_addr dest_mac;
	if (!arp_cache_lookup(ip_hdr->dest, &dest_mac)) {
		pk_drop(pk);
		return;
	}
	eth_hdr->dest = dest_mac;

	// send the packet to the next hop
	NET_SEND(neighbor->nif, pk);

	// free the packet
	pk_free(pk);
}
