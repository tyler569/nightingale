
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

void print_from_fd_forever(int fd) {
    static char buffer[1025];
    while (true) {
        int len = read(fd, buffer, 1024);
        if (len < 0) {
            perror("socket read()");
            exit(1);
        }
        buffer[len] = 0;
        printf("%s", buffer);
    }
}

void send_stdin_to_fd_forever(int fd) {
    static char buffer[1025];
    while (true) {
        // TODO: line buffer input
        int len = read(stdin_fd, buffer, 1024);
        if (len < 0) {
            perror("stdin read()");
            exit(1);
        }
        buffer[len] = 0;
        printf("%s", buffer);

        int written = write(fd, buffer, len);
        if (written < 0) {
            perror("write()");
        }
        if (written != len) {
            printf("failed to send all %i (sent %i)\n", len, written);
        }
    }
}

int main(int argc, char** argv) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, PROTO_UDP);

    struct sockaddr_in addr = {
        AF_INET, 1025, { 0x0a00020f /* 10.0.2.15 */ }, {0}
    };
    struct sockaddr_in dest = {
        AF_INET, 1330, { 0x0a000202 /* 10.0.2.2 */ }, {0}
    };

    int status;

    status =
        bind(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (status < 0) {
        perror("bind()");
        return 1;
    }
    status =
        connect(sock_fd, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));
    if (status < 0) {
        perror("connect()");
        return 1;
    }

    status = fork();
    if (status == 0) {
        print_from_fd_forever(sock_fd);
    } else if (status < 0) {
        perror("fork()");
        return 1;
    }

    status = fork();
    if (status == 0) {
        send_stdin_to_fd_forever(sock_fd);
    } else if (status < 0) {
        perror("fork()");
        return 1;
    }

    int output;
    waitpid(status, &output, 0);

    return 0;
}

