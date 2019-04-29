
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void print_my_letter(char c) {
        for (int j = 0; j < 10; j++) {
                /*for (int i = 0; i < 1000000; i++) {
                }*/
                printf("%c", c);
        }
        exit(0);
}

int main() {
        setpgid();

        for (char c = 'A'; c <= 'Z'; c++) {
                if (!fork()) {
                        print_my_letter(c);
                }
        }

        while (errno != ECHILD) {
                // collect all the zombies

                int pid = getpid();
                int status = 0;
                waitpid(-pid, &status, WNOHANG);
        }

        return 0;
}
