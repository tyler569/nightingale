#pragma once

#include <netinet/in.h>
#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

// Hook points
enum net_hook {
	NF_INET_PRE_ROUTING = 0,
	NF_INET_LOCAL_IN = 1,
	NF_INET_FORWARD = 2,
	NF_INET_LOCAL_OUT = 3,
	NF_INET_POST_ROUTING = 4,
	NF_INET_NUMHOOKS = 5
};

// Verdicts
enum nf_verdict {
	NF_DROP = 0,
	NF_ACCEPT = 1,
	NF_STOLEN = 2,
	NF_QUEUE = 3,
	NF_REPEAT = 4,
	NF_STOP = 5
};

// Match flags
#define MATCH_SRC_IP (1 << 0)
#define MATCH_DEST_IP (1 << 1)
#define MATCH_SRC_PORT (1 << 2)
#define MATCH_DEST_PORT (1 << 3)
#define MATCH_PROTOCOL (1 << 4)

// Rule match criteria
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

// Rule target
struct net_target {
	enum nf_verdict verdict;
};

// A filtering rule
struct net_rule {
	struct net_match match;
	struct net_target target;
	uint32_t priority;
	struct net_rule *next;
	char name[32];
};

// Statistics structure
struct nf_stats {
	uint64_t hook_counts[NF_INET_NUMHOOKS];
	uint64_t accept_count;
	uint64_t drop_count;
};

#ifndef __kernel__
// Syscall prototypes
int netfilter_add_rule(int hook, const struct net_rule *rule);
int netfilter_remove_rule(int hook, const char *name);
int netfilter_get_stats(void *stats_buf);
#endif

END_DECLS