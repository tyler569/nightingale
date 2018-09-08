
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {

    strace(1);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);
    bind0(sock_fd, 0x0a00000a /* 10.0.0.10 */, 4);
    connect0(sock_fd, 0x0a00000b /* 10.0.0.11 */, 1330);

    write(sock_fd, "Hello World\n", 12);
    printf("data sent\n");
    char recv[128] = {0};
    read(sock_fd, recv, 128);
    printf("received: '%s'\n", recv);

}

