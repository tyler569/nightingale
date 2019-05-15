
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <nightingale.h>

int main(int, char **);

void malloc_init(void);

int _start(int argc, char **argv, char **envp) {
        malloc_init();

        int retval = main(argc, argv);
        exit(retval);
}
