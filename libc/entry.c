#include <fcntl.h>
#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int, char **);

void __nc_f_init(void);
void __nc_f_fini(void);

void __nc_init(void)
{
    __nc_malloc_init();
    __nc_f_init();
}

void __nc_fini(void) { __nc_f_fini(); }

int __nc_start(int argc, char **argv, char **envp)
{
    int retval = main(argc, argv);
    __nc_fini();
    exit(retval);
}
