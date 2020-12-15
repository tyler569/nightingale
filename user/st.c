#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    int err;
    err = unlink("socket2");
    if (err && errno != ENOENT) {
        perror("unlink");
        return 1;
    }
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    struct sockaddr_un address = {
        .sun_family = AF_UNIX,
        .sun_path = "socket2",
    };
    err = bind(sock, (struct sockaddr *)&address, sizeof(address));
    if (err) {
        perror("bind");
        return 1;
    }
    err = listen(sock, 1);
    if (err) {
        perror("listen");
        return 1;
    }

    while (true) {
        struct sockaddr_un recv_addr;
        socklen_t recv_len = sizeof(recv_addr);

        int ls = accept(sock, (struct sockaddr *)&recv_addr, &recv_len);
        if (ls < 0) {
            perror("accept");
            return 1;
        }

        while (true) {
            char buffer_recv[128] = {0};
            int len = recv(ls, buffer_recv, 128, 0);
            if (len < 0) {
                perror("recv");
                return 1;
            }
            if (len == 0) break;
            printf("recv: %s", buffer_recv);
        }
    }
}
