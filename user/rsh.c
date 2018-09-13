
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);

    struct sockaddr_in addr = { AF_INET, 1025, { 0x0a00020f /* 10.0.2.15 */ } };
    struct sockaddr_in dest = { AF_INET, 1330, { 0x0a000202 /* 10.0.2.2 */ } };

    bind(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    connect(sock_fd, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));

    dup2(sock_fd, 0);
    dup2(sock_fd, 1);
    dup2(sock_fd, 2);

    if (fork() == 0) {
        char* argv0 = NULL;
        execve("init", &argv0, NULL);
    }

    return 0;
}

