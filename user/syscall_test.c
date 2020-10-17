#include <stdio.h>

int main() {
    int syscall_test(char *buffer);
    char buffer[128] = {0};

    syscall_test(buffer);
    printf("And the big reveal: '%s'\n", buffer);
}
