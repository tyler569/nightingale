
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "vfs.h"

uint64_t tar_number_convert(char *num) {
    size_t len = strlen(num);
    uint64_t value = 0;
    
    for (size_t place=0; place<=len; place += 1) {
        value += (num[place] - '0') << ((len - place) * 3);
    }

    return value;
}


