
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

void write_to_pipe(int fd) {
        char buf[256] = {0};
        while (true) {
                int len = read(STDIN_FILENO, buf, 256);
                if (len == 0)  break;
                write(fd, buf, len);
        }
        close(fd);
}

void read_from_pipe(int fd) {
        char buf[256] = {0};
        while (true) {
                int len = read(fd, buf, 256);
                if (len == 0)  break;
                buf[len] = 0;
                printf("recieved: '%s'\n", buf);
        }
        close(fd);
}

int main() {
        int fds[2] = {0};
        pipe(fds);

        pid_t child = fork();
        if (child) {
                printf("writer: %i\n", getpid());
                write_to_pipe(fds[1]);
        } else {
                printf("reader: %i\n", getpid());
                read_from_pipe(fds[0]);
                exit(1);
        }

        int status;
        waitpid(child, &status, 0);

        printf("child exited with %i\n", status);

        return 0;
}

