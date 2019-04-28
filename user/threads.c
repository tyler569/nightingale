
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_my_letter(char c) {
        for (int j = 0; j < 10; j++) {
                for (int i = 0; i < 1000000; i++) {
                }
                printf("%c", c);
        }
        exit(0);
}

int main() {
        setpgid();

        for (int i = 0; i < 5; i++) {
                for (char c = '@'; c < 'z'; c++) {
                        if (!fork()) {
                                print_my_letter(c);
                        }
                }
        }

        print_my_letter('z');

        while (true) {
                // collect all the zombies

                int pid = getpid();
                int status = 0;
                if (waitpid(-pid, &status, WNOHANG) == 0) {
                        return 0;
                }
        }

        return 0;
}
