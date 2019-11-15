
/* Test program that tries to register a signal handler. You should be
 * able to type C-x at the console to send SIGINT and have this program
 * print a helpful message
 */

#include <stdio.h>
#include <signal.h>

// TODO: this could be in nc probably.
const char *signal_names[] = {
        [SIGINT]        = "SIGINT",
};

void signal_handler(int signal_number) {
        printf("This is a signal handler\n");
        printf("Signal recieved: %s\n", signal_names[signal_number]);
}

int main(int argc, char **argv) {
        signal(SIGINT, signal_handler);

        char buf[256] = {0};

        while (!feof(stdin)) {
                fgets(buf, 256, stdin);
        }

        printf("Got EOF on stdin\n");
        return 0;
}

