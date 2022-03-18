#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

int serve()
{
    int err;
    err = unlink("socket2");
    if (err && errno != ENOENT) {
        perror("unlink");
        exit(1);
    }
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_un address = {
        .sun_family = AF_UNIX,
        .sun_path = "socket2",
    };
    err = bind(sock, (struct sockaddr *)&address, sizeof(address));
    if (err) {
        perror("bind");
        exit(1);
    }
    err = listen(sock, 1);
    if (err) {
        perror("listen");
        exit(1);
    }

    while (true) {
        struct sockaddr_un recv_addr;
        socklen_t recv_len = sizeof(recv_addr);

        int ls = accept(sock, (struct sockaddr *)&recv_addr, &recv_len);
        if (ls < 0) {
            perror("accept");
            exit(1);
        }
        printf("server accepted: %i\n", ls);

        while (true) {
            char buffer_recv[128] = { 0 };
            int len = recv(ls, buffer_recv, 128, 0);
            if (len < 0) {
                perror("recv");
                exit(1);
            }
            if (len == 0)
                break;
            printf("server recv: %i %s", len, buffer_recv);
            len = send(ls, buffer_recv, len, 0);
            if (len < 0) {
                perror("send");
                exit(1);
            }
            printf("server send: %i %s", len, buffer_recv);
        }
    }
}

int main()
{
    pid_t pid;
    if ((pid = fork()) == 0) {
        serve();
    } else {
        printf("server serving at %i\n", pid);
    }
    return 0;
}
