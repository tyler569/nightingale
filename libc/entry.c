
#include <unistd.h>

int main(int, char**);

int _start(int argc, char **argv, char **envp) {
    int retval = main(argc, argv);
    exit(retval);
}

