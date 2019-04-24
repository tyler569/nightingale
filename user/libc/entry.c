
#include <unistd.h>

int main(int, char **);

extern char const *lower_hex_charset;
extern void raw_print(char const *, int);

void malloc_init(void);

int _start(int argc, char **argv, char **envp) {
        malloc_init();

        int retval = main(argc, argv);
        exit_group(retval);
}
