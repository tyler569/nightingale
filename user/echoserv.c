
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

const size_t addrlen = sizeof(struct sockaddr_in);

int main() {
        int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock_fd < 0) { perror("socket()"); exit(1); }

        struct sockaddr_in addr = {
            AF_INET, 1025, {0 /* 0.0.0.0 */ }, {0}
        };

        int ret;
        ret = bind(sock_fd, (struct sockaddr *)&addr, addrlen);
        if (ret < 0) { perror("bind()"); exit(2); }

        char recv_buf[128] = {0};
        struct sockaddr_in remote_addr;
        while (true) {
                size_t x_addrlen = addrlen;

                ssize_t len = recvfrom(sock_fd, recv_buf, 128, 0,
                        (struct sockaddr *)&remote_addr, &x_addrlen);
                if (len < 0) { perror("recvfrom()"); exit(1); }

                printf("from: port: %hu\n", remote_addr.sin_port);

                recv_buf[len] = '\0';
                printf("received: %li '%s'\n", len, recv_buf);

                int err = sendto(sock_fd, recv_buf, len, 0,
                        (struct sockaddr *)&remote_addr, addrlen);
                if (len < 0) { perror("sendto()"); exit(1); }
        }
}

