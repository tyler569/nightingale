
#include <unistd.h>

int main(int, char**);

extern char const* lower_hex_charset;
extern void raw_print(char const*, int);

int _start(int argc, char **argv, char **envp) {
    int retval = main(argc, argv);
    exit(retval);
}

