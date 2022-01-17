#include <fcntl.h>
#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int, char **);

void initialize_standard_library() {
    nc_malloc_init();
}

int nc_start(int argc, char **argv, char **envp) {
    int retval = main(argc, argv);
    // TODO: fclose all open files
    fclose(stdout);
    fclose(stderr);
    exit(retval);
}
