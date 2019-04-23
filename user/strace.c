
#include <unistd.h>

int main(int argc, char **argv) {
        strace(1);
        int success = execve(argv[1], argv + 2, NULL);
        if (success) {
                // perror("execve()");
                return 1;
        }
        return 0;
}
