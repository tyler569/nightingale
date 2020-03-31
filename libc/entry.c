
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <nightingale.h>

int main(int, char **);

int _start(int argc, char **argv, char **envp) {
        malloc_initialize(NULL, (16 * 1024*1024));

        int retval = main(argc, argv);
        exit(retval);
}

