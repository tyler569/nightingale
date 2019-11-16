
#include <stdio.h>
#include <unistd.h>

int main() {
    fork();

    pid_t pid = getpid();

    printf("This is process %i\n", pid);

    char c = getchar();

    printf("pid: %i, char: '%c' (%hhi)\n", pid, c, c);

    return 0;
}

