#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <nightingale.h>
#include <unistd.h>

int main(int, char **);

void __nc_init() { __nc_malloc_init(); }

int __nc_start(int argc, char **argv, char **envp)
{
    int retval = main(argc, argv);
    // TODO: fclose all open files
    fclose(stdout);
    fclose(stderr);
    exit(retval);
}
