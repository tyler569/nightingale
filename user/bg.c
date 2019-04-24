
#include <stdio.h>
#include <unistd.h>

int main() {
        if (fork()) {
                return 1;
        }

        while(true) {
        }
}
