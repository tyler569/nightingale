#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void test_udp_client() {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("socket failed\n");
		return;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(12345),
		.sin_addr.s_addr = 0x7f000001 // 127.0.0.1 in network byte order
	};

	const char *message = "Hello from UDP client!";

	printf("Sending UDP message: %s\n", message);

	ssize_t sent = sendto(sock, message, strlen(message), 0,
		(struct sockaddr *)&server_addr, sizeof(server_addr));

	if (sent < 0) {
		printf("sendto failed\n");
	} else {
		printf("Sent %zd bytes\n", sent);
	}

	close(sock);
}

void test_udp_server() {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("socket failed\n");
		return;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(12345),
		.sin_addr.s_addr = 0 // INADDR_ANY
	};

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("bind failed\n");
		close(sock);
		return;
	}

	printf("UDP server listening on port 12345\n");

	char buffer[1024];
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	ssize_t received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
		(struct sockaddr *)&client_addr, &client_len);

	if (received < 0) {
		printf("recvfrom failed\n");
	} else {
		buffer[received] = '\0';
		printf("Received %zd bytes: %s\n", received, buffer);
		printf("From: %08x:%d\n", client_addr.sin_addr.s_addr,
			ntohs(client_addr.sin_port));
	}

	close(sock);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <client|server>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "client") == 0) {
		test_udp_client();
	} else if (strcmp(argv[1], "server") == 0) {
		test_udp_server();
	} else {
		printf("Unknown mode: %s\n", argv[1]);
		printf("Usage: %s <client|server>\n", argv[0]);
		return 1;
	}

	return 0;
}