
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("this is pid: %i, tid: %i\n", getpid(), gettid());
    return 0;
}

