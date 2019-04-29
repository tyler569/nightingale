
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

int main() {
        if (fork()) {
                return 1;
        }

        int i = 0;
        while(true) {
                if (i % 10000 == 0) {
                        printf(".");
                }
                i++;
                yield();
        }
}
