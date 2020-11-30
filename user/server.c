#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int err;
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    int sock2 = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock2 < 0) {
        perror("socket");
        return 1;
    }
    struct sockaddr_un address = {
        .sun_family = AF_UNIX,
        .sun_path = "socket", // "./socket",
    };
    err = bind(sock, (struct sockaddr *)&address, sizeof(address));
    if (err < 0) {
        perror("bind");
        return 1;
    }
    char buffer[128] = "Hello World\n";
    err = sendto(sock2, buffer, 128, 0, (struct sockaddr *)&address, sizeof(address));
    if (err < 0) {
        perror("sendto");
        return 1;
    }
    char buffer_recv[128] = {0};
    struct sockaddr_un recv_addr;
    socklen_t recv_len = sizeof(recv_addr);
    err = recvfrom(sock, buffer_recv, 128, 0, (struct sockaddr *)&recv_addr, &recv_len);
    if (err < 0) {
        perror("recvfrom");
        return 1;
    }
    printf("message: '%s'\n", buffer_recv);
}
