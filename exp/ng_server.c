#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

void check_err(int code, const char *message) {
    if (code < 0) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

int main() {
    int sock, err, len;
    int local_addr = 1;
    int remote_addr;
    socklen_t remote_addrlen;
    char buffer[1024];

    sock = socket(AF_NG, SOCK_DGRAM, PROTO_NG);
    check_err(sock, "socket");

    err = bind(sock, &local_addr, sizeof(local_addr));
    check_err(err, "bind");

    while (true) {
        len = recvfrom(sock, buffer, 1024, &remote_addr, &remote_addrlen);
        check_err(len, "recvfrom");

        printf("server: recieved buffer '%s' from %i\n", buffer, remote_addr);

        // echo back to client
        err = sendto(sock, buffer, len, &remote_addr, remote_addrlen);
        check_err(err, "sendto");
    }
}
