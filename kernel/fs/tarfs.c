
#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
}

uint64_t tar_number_convert(char *num) {
    size_t len = strlen(num);
    uint64_t value = 0;
    
    for (size_t place=0; place<=len; place += 1) {
        value += (num[place] - '0') << ((len - place) * 3);
    }

    return value;
}


