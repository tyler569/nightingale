#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <netinet/in.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <ng/sync.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Hook points - similar to netfilter
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

#define MATCH_SRC_IP (1 << 0)
#define MATCH_DEST_IP (1 << 1)
#define MATCH_SRC_PORT (1 << 2)
#define MATCH_DEST_PORT (1 << 3)
#define MATCH_PROTOCOL (1 << 4)

// Rule target
struct net_target {
	enum nf_verdict verdict;
	// Future: could add jump targets, NAT info, etc.
};

// A filtering rule
struct net_rule {
	struct net_match match;
	struct net_target target;
	uint32_t priority;
	struct net_rule *next;
	char name[32];
};

// Rule chains for each hook
static struct net_rule *rule_chains[NF_INET_NUMHOOKS];
static spinlock_t filter_lock = {};

// Statistics
static uint64_t hook_stats[NF_INET_NUMHOOKS] = { 0 };
static uint64_t drop_count = 0;
static uint64_t accept_count = 0;

static bool match_ip(
	struct in_addr addr, struct in_addr match, struct in_addr mask) {
	return (addr.s_addr & mask.s_addr) == (match.s_addr & mask.s_addr);
}

static bool match_port(uint16_t port, uint16_t min, uint16_t max) {
	return port >= min && port <= max;
}

static enum nf_verdict evaluate_rule(struct net_rule *rule, struct pk *pk) {
	struct ip_hdr *ip = L3(pk);
	struct net_match *match = &rule->match;

	// Check IP addresses
	if ((match->flags & MATCH_SRC_IP)
		&& !match_ip(ip->src, match->src_ip, match->src_mask)) {
		return NF_ACCEPT; // Rule doesn't match, continue
	}

	if ((match->flags & MATCH_DEST_IP)
		&& !match_ip(ip->dest, match->dest_ip, match->dest_mask)) {
		return NF_ACCEPT;
	}

	// Check protocol
	if ((match->flags & MATCH_PROTOCOL) && ip->protocol != match->protocol) {
		return NF_ACCEPT;
	}

	// Check ports for TCP/UDP
	if ((match->flags & (MATCH_SRC_PORT | MATCH_DEST_PORT))
		&& (ip->protocol == IPPROTO_TCP || ip->protocol == IPPROTO_UDP)) {

		uint16_t src_port, dest_port;

		if (ip->protocol == IPPROTO_UDP) {
			struct udp_hdr *udp = L4(pk);
			src_port = ntohs(udp->src_port);
			dest_port = ntohs(udp->dest_port);
		} else {
			struct tcp_hdr *tcp = L4(pk);
			src_port = ntohs(tcp->src_port);
			dest_port = ntohs(tcp->dest_port);
		}

		if ((match->flags & MATCH_SRC_PORT)
			&& !match_port(
				src_port, match->src_port_min, match->src_port_max)) {
			return NF_ACCEPT;
		}

		if ((match->flags & MATCH_DEST_PORT)
			&& !match_port(
				dest_port, match->dest_port_min, match->dest_port_max)) {
			return NF_ACCEPT;
		}
	}

	// Rule matches, return target verdict
	printf("Firewall rule '%s' matched, verdict: %s\n", rule->name,
		rule->target.verdict == NF_ACCEPT ? "ACCEPT" : "DROP");

	return rule->target.verdict;
}

enum nf_verdict nf_hook(enum net_hook hook, struct pk *pk) {
	hook_stats[hook]++;

	spin_lock(&filter_lock);

	struct net_rule *rule = rule_chains[hook];
	enum nf_verdict verdict = NF_ACCEPT; // Default policy

	while (rule) {
		verdict = evaluate_rule(rule, pk);
		if (verdict != NF_ACCEPT) {
			break; // Rule matched and gave a definitive verdict
		}
		rule = rule->next;
	}

	spin_unlock(&filter_lock);

	if (verdict == NF_DROP) {
		drop_count++;
	} else if (verdict == NF_ACCEPT) {
		accept_count++;
	}

	return verdict;
}

int nf_add_rule(enum net_hook hook, struct net_rule *new_rule) {
	if (hook >= NF_INET_NUMHOOKS) {
		return -1;
	}

	spin_lock(&filter_lock);

	// Insert rule in priority order (lower priority number = higher precedence)
	struct net_rule **current = &rule_chains[hook];
	while (*current && (*current)->priority <= new_rule->priority) {
		current = &(*current)->next;
	}

	new_rule->next = *current;
	*current = new_rule;

	spin_unlock(&filter_lock);

	printf("Added firewall rule '%s' to hook %d with priority %u\n",
		new_rule->name, hook, new_rule->priority);

	return 0;
}

int nf_remove_rule(enum net_hook hook, const char *name) {
	if (hook >= NF_INET_NUMHOOKS) {
		return -1;
	}

	spin_lock(&filter_lock);

	struct net_rule **current = &rule_chains[hook];
	while (*current) {
		if (strcmp((*current)->name, name) == 0) {
			struct net_rule *to_remove = *current;
			*current = to_remove->next;
			spin_unlock(&filter_lock);
			free(to_remove);
			printf("Removed firewall rule '%s' from hook %d\n", name, hook);
			return 0;
		}
		current = &(*current)->next;
	}

	spin_unlock(&filter_lock);
	return -1; // Rule not found
}

// Helper function to create common rules
struct net_rule *create_drop_rule(const char *name, uint32_t priority,
	struct in_addr src, struct in_addr src_mask) {
	struct net_rule *rule = malloc(sizeof(struct net_rule));
	if (!rule)
		return nullptr;

	memset(rule, 0, sizeof(struct net_rule));
	strncpy(rule->name, name, sizeof(rule->name) - 1);
	rule->priority = priority;
	rule->match.src_ip = src;
	rule->match.src_mask = src_mask;
	rule->match.flags = MATCH_SRC_IP;
	rule->target.verdict = NF_DROP;

	return rule;
}

struct net_rule *create_port_block_rule(
	const char *name, uint32_t priority, uint16_t port, uint8_t protocol) {
	struct net_rule *rule = malloc(sizeof(struct net_rule));
	if (!rule)
		return nullptr;

	memset(rule, 0, sizeof(struct net_rule));
	strncpy(rule->name, name, sizeof(rule->name) - 1);
	rule->priority = priority;
	rule->match.dest_port_min = port;
	rule->match.dest_port_max = port;
	rule->match.protocol = protocol;
	rule->match.flags = MATCH_DEST_PORT | MATCH_PROTOCOL;
	rule->target.verdict = NF_DROP;

	return rule;
}

void nf_print_stats() {
	printf("Netfilter statistics:\n");
	printf("  PREROUTING:  %lu packets\n", hook_stats[NF_INET_PRE_ROUTING]);
	printf("  LOCAL_IN:    %lu packets\n", hook_stats[NF_INET_LOCAL_IN]);
	printf("  FORWARD:     %lu packets\n", hook_stats[NF_INET_FORWARD]);
	printf("  LOCAL_OUT:   %lu packets\n", hook_stats[NF_INET_LOCAL_OUT]);
	printf("  POSTROUTING: %lu packets\n", hook_stats[NF_INET_POST_ROUTING]);
	printf("  Accepted:    %lu packets\n", accept_count);
	printf("  Dropped:     %lu packets\n", drop_count);
}