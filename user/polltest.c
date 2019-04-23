
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv) {

        struct pollfd fds = {0, POLLIN, 0};

        while (true) {
                int status = poll(&fds, 1, 1000);
                if (status) {
                        char c = getchar();
                        printf("%c", c);
                } else {
                        printf(".");
                }
        }
}
