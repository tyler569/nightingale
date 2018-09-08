
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {

    strace(1);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);
    bind0(sock_fd, 0x0a00020f /* 10.0.2.15 */, 4);
    connect0(sock_fd, 0x0a000202 /* 10.0.2.2 */, 1330);

    write(sock_fd, "Hello World\n", 12);
    printf("data sent\n");
    char recv[128] = {0};
    read(sock_fd, recv, 128);
    printf("received: '%s'\n", recv);

}

