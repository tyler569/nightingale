#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <ng/net.h>
#include <ng/pk.h>

struct pk *net_ingress_head = nullptr;
struct pk *net_ingress_tail = nullptr;

void net_worker() {
	while (true) { }
}