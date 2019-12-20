
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void print_my_letter(char c) {
        for (int j=0; j<10; j++) {
                for (int i=0; i<500000; i++);

                printf("%c", c);
        }
        exit(0);
}

int main() {
        int pid = getpid();
        setpgid(pid, pid);

        for (char c='A'; c<='Z'; c++) {
                int child;
                if (!(child = fork())) {
                        print_my_letter(c);
                        continue;
                }
                waitpid(child, &child, 0);
        }

        while (errno != ECHILD) {
                // collect all the zombies

                int status = 0;
                waitpid(-pid, &status, 0);
        }

        printf("\n");

        return 0;
}

