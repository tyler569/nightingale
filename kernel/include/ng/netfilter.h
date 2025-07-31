#pragma once

#include <netinet/in.h>
#include <ng/pk.h>
#include <sys/cdefs.h>

BEGIN_DECLS

enum net_hook {
	NF_INET_PRE_ROUTING = 0,
	NF_INET_LOCAL_IN = 1,
	NF_INET_FORWARD = 2,
	NF_INET_LOCAL_OUT = 3,
	NF_INET_POST_ROUTING = 4,
	NF_INET_NUMHOOKS = 5
};

enum nf_verdict {
	NF_DROP = 0,
	NF_ACCEPT = 1,
	NF_STOLEN = 2,
	NF_QUEUE = 3,
	NF_REPEAT = 4,
	NF_STOP = 5
};

struct net_match {
	struct in_addr src_ip;
	struct in_addr src_mask;
	struct in_addr dest_ip;
	struct in_addr dest_mask;
	uint16_t src_port_min;
	uint16_t src_port_max;
	uint16_t dest_port_min;
	uint16_t dest_port_max;
	uint8_t protocol;
	uint8_t flags;
};

struct net_target {
	enum nf_verdict verdict;
};

struct net_rule {
	struct net_match match;
	struct net_target target;
	uint32_t priority;
	struct net_rule *next;
	char name[32];
};

// Core filtering functions
enum nf_verdict nf_hook(enum net_hook hook, struct pk *pk);
int nf_add_rule(enum net_hook hook, struct net_rule *rule);
int nf_remove_rule(enum net_hook hook, const char *name);

// Helper functions
struct net_rule *create_drop_rule(const char *name, uint32_t priority,
	struct in_addr src, struct in_addr src_mask);
struct net_rule *create_port_block_rule(
	const char *name, uint32_t priority, uint16_t port, uint8_t protocol);

void nf_print_stats();

END_DECLS