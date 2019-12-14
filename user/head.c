// vim: sw=4:ts=4:sts=4 :
#include <stdio.h>

int main() {
    char buffer[1024];
    char output[1024];
    size_t len;

    int n_nl = 10;

    while (n_nl > 0 && !feof(stdin)) {
        len = fread(buffer, 1, 1024, stdin);
        int i;
        for (i=0; i<len; i++) {
            if (buffer[i] == '\n') {
                n_nl -= 1;
            }
            output[i] = buffer[i];
            if (n_nl == 0) {
                i += 1;
                break;
            }
        }
        fwrite(output, 1, i, stdout);
    }
    return 0;
}

