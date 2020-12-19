#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int traceback(pid_t tid, char *buffer, size_t len);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "An argument is required - traceback [pid]\n");
        return 1;
    }
    pid_t tid = strtol(argv[1], NULL, 10);
    char buffer[1000];
    traceback(tid, buffer, 1000);
    printf("%s", buffer);
}
