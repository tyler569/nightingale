#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    int err;
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    struct sockaddr_un address = {
        .sun_family = AF_UNIX,
        .sun_path = "socket2",
    };
    err = connect(sock, (struct sockaddr *)&address, sizeof(address));
    if (err) {
        perror("connect");
        return 1;
    }

    char buffer[256];
    while (fgets(buffer, 256, stdin)) {
        int len;
        len = send(sock, buffer, strlen(buffer), 0);
        if (err < 0) {
            perror("send");
            break;
        }
        printf("client send: %s", buffer);
        len = recv(sock, buffer, 256, 0);
        if (len < 0) {
            perror("recv");
            break;
        }
        printf("client recv: %i %s", len, buffer);
    }
    return 0;
}
