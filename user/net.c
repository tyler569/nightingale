
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {
    strace(1);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);

    struct sockaddr_in addr = { AF_INET, 1025, { 0x0a00020f /* 10.0.2.15 */ } };
    struct sockaddr_in dest = { AF_INET, 1330, { 0x0a000202 /* 10.0.2.2 */ } };

    bind(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    connect(sock_fd, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));

    send(sock_fd, "Hello World\n", 12, 0);
    printf("Sent Hello World\n");

    char recv_buf[128] = {0};
    while (true) {
        size_t len = recv(sock_fd, recv_buf, 128, 0);
        recv_buf[len] = '\0';
        printf("received: %lu '%s'\n", len, recv_buf);
        send(sock_fd, recv_buf, len, 0);
    }
}

