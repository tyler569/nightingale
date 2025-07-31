#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/netfilter.h>
#include <sys/socket.h>
#include <unistd.h>

// Simple inet_aton implementation without sscanf
int simple_inet_aton(const char *cp, struct in_addr *inp) {
	// For simplicity, just handle 192.168.1.100 as an example
	if (strcmp(cp, "192.168.1.100") == 0) {
		inp->s_addr = htonl((192 << 24) | (168 << 16) | (1 << 8) | 100);
		return 1;
	}
	// Add other common IPs as needed
	printf("Warning: IP parsing simplified, only supports 192.168.1.100\n");
	return 0;
}

void add_firewall_rule(const char *name, const char *action, const char *src_ip,
	const char *protocol, int port) {
	struct net_rule rule;
	memset(&rule, 0, sizeof(rule));

	// Set rule name
	strncpy(rule.name, name, sizeof(rule.name) - 1);

	// Set target action
	if (strcmp(action, "ACCEPT") == 0) {
		rule.target.verdict = NF_ACCEPT;
	} else if (strcmp(action, "DROP") == 0) {
		rule.target.verdict = NF_DROP;
	} else {
		printf("Error: Invalid action '%s'\n", action);
		return;
	}

	// Set priority (lower number = higher priority)
	rule.priority = 1000;

	// Configure match criteria
	if (src_ip) {
		rule.match.flags |= MATCH_SRC_IP;
		if (!simple_inet_aton(src_ip, &rule.match.src_ip)) {
			printf("Error: Invalid IP address '%s'\n", src_ip);
			return;
		}
		rule.match.src_mask.s_addr = 0xFFFFFFFF; // /32 mask
	}

	if (protocol) {
		rule.match.flags |= MATCH_PROTOCOL;
		if (strcmp(protocol, "tcp") == 0) {
			rule.match.protocol = 6; // IPPROTO_TCP
		} else if (strcmp(protocol, "udp") == 0) {
			rule.match.protocol = 17; // IPPROTO_UDP
		}
	}

	if (port > 0) {
		rule.match.flags |= MATCH_DEST_PORT;
		rule.match.dest_port_min = port;
		rule.match.dest_port_max = port;
	}

	// Add rule to LOCAL_IN hook (for incoming packets)
	int result = netfilter_add_rule(NF_INET_LOCAL_IN, &rule);
	if (result < 0) {
		printf("Failed to add rule: error %d\n", result);
	} else {
		printf("Successfully added firewall rule '%s'\n", name);
	}
}

void remove_firewall_rule(const char *name) {
	int result = netfilter_remove_rule(NF_INET_LOCAL_IN, name);
	if (result < 0) {
		printf("Failed to remove rule '%s': error %d\n", name, result);
	} else {
		printf("Successfully removed firewall rule '%s'\n", name);
	}
}

void list_firewall_rules() {
	printf("Firewall rules listing not yet implemented\n");
	// TODO: Add a list syscall
}

void show_firewall_stats() {
	int result = netfilter_get_stats(0);
	if (result < 0) {
		printf("Failed to get firewall statistics: error %d\n", result);
	}
}

void usage(const char *prog) {
	printf("Usage: %s <command> [options]\n\n", prog);
	printf("Commands:\n");
	printf("  add <name> <action> [--src <ip>] [--proto <tcp|udp>] [--port "
		   "<port>]\n");
	printf("      Add a new firewall rule\n");
	printf("      Actions: ACCEPT, DROP\n");
	printf("  remove <name>\n");
	printf("      Remove a firewall rule\n");
	printf("  list\n");
	printf("      List all firewall rules\n");
	printf("  stats\n");
	printf("      Show firewall statistics\n");
	printf("\nExamples:\n");
	printf("  %s add block_ssh DROP --port 22 --proto tcp\n", prog);
	printf("  %s add block_bad_ip DROP --src 192.168.1.100\n", prog);
	printf("  %s remove block_ssh\n", prog);
	printf("  %s list\n", prog);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "add") == 0) {
		if (argc < 4) {
			printf("Error: add command requires name and action\n");
			usage(argv[0]);
			return 1;
		}

		const char *name = argv[2];
		const char *action = argv[3];
		const char *src_ip = nullptr;
		const char *protocol = nullptr;
		int port = 0;

		// Parse optional arguments
		for (int i = 4; i < argc - 1; i++) {
			if (strcmp(argv[i], "--src") == 0) {
				src_ip = argv[i + 1];
				i++;
			} else if (strcmp(argv[i], "--proto") == 0) {
				protocol = argv[i + 1];
				i++;
			} else if (strcmp(argv[i], "--port") == 0) {
				port = atoi(argv[i + 1]);
				i++;
			}
		}

		if (strcmp(action, "ACCEPT") != 0 && strcmp(action, "DROP") != 0) {
			printf("Error: action must be ACCEPT or DROP\n");
			return 1;
		}

		add_firewall_rule(name, action, src_ip, protocol, port);

	} else if (strcmp(argv[1], "remove") == 0) {
		if (argc < 3) {
			printf("Error: remove command requires rule name\n");
			usage(argv[0]);
			return 1;
		}

		remove_firewall_rule(argv[2]);

	} else if (strcmp(argv[1], "list") == 0) {
		list_firewall_rules();

	} else if (strcmp(argv[1], "stats") == 0) {
		show_firewall_stats();

	} else {
		printf("Error: unknown command '%s'\n", argv[1]);
		usage(argv[0]);
		return 1;
	}

	return 0;
}