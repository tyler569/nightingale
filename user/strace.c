
#include <unistd.h>

int main(int argc, char **argv) {
        strace(1);
        return execve(argv[1], argv + 1, NULL);
}

