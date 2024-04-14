#include <net/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>

void ip_ingress(struct pk *pk) {
	struct ip_hdr *ip = (struct ip_hdr *)(pk->data + pk->l3_offset);

	if (ip->ver_ihl != 0x45) {
		pk_drop(pk);
		return;
	}
}
