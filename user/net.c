
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main() {
        int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock_fd < 0) {
                perror("socket()");
                exit(1);
        }

        struct sockaddr_in addr = {
            AF_INET, 1025, {0x0a00020f /* 10.0.2.15 */}, {0}
        };
        struct sockaddr_in dest = {
            AF_INET, 1330, {0x0a000202 /* 10.0.2.2 */}, {0}
        };

        int ret;
        ret = bind(sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        if (ret < 0) {
                perror("bind()");
                exit(2);
        }

        ret = connect(sock_fd, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
        if (ret < 0) {
                perror("connect()");
                exit(3);
        }

        send(sock_fd, "Hello World\n", 12, 0);
        printf("Sent Hello World\n");

        char recv_buf[128] = {0};
        while (true) {
                // size_t len = recv(sock_fd, recv_buf, 128, 0);
                ssize_t len = read(sock_fd, recv_buf, 128);
                if (len < 0) {
                        perror("read()");
                        exit(1);
                }
                recv_buf[len] = '\0';
                printf("received: %li '%s'\n", len, recv_buf);

                int err = send(sock_fd, recv_buf, len, 0);
                if (len < 0) {
                        perror("send()");
                        exit(1);
                }
        }
}

