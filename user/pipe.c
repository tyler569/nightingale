
#include <stdio.h>
#include <unistd.h>

void write_to_pipe(int fd) {
        char buf[256] = {0};
        while (true) {
                int len = read(STDIN_FILENO, buf, 256);
                if (len == 0)  exit(0);
                write(fd, buf, len);
        }
}

void read_from_pipe(int fd) {
        char buf[256] = {0};
        while (true) {
                int len = read(fd, buf, 256);
                if (len == 0)  exit(0);
                buf[len] = 0;
                printf("recieved: '%s'\n", buf);
        }
}

int main() {
        int fds[2] = {0};
        pipe(fds);

        if (fork()) {
                write_to_pipe(fds[1]);
        } else {
                read_from_pipe(fds[0]);
        }

        return 0;
}

