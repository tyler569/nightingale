
#include <stdio.h>
#include <signal.h>

void sigint_handler(int signal) {
        printf("This is a sigint handler\n");
}

int main() {
        signal(SIGINT, sigint_handler);

        raise(SIGINT);

        // fgetc(stdin);
}
