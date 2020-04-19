
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <nightingale.h>

int main(int, char **);

void initialize_standard_library() {
        malloc_initialize(NULL, (16 * 1024*1024));
}

int nc_start(int argc, char **argv, char **envp) {
        int retval = main(argc, argv);
        exit(retval);
}

