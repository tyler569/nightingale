
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <print.h>
#include <vmm.h>
#include "elf.h"

int check_elf(struct elf_header *elf) {
    uint8_t working_ident[8] = {
        0x7F, 'E', 'L', 'F',
        ELF64, ELFLE, ELFABI, ELFEXEC
    };

    if (memcmp(elf, working_ident, 8) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int load_elf(struct elf_header *elf) {
    //
    // something something
    // vmm_create_unbacked(PAGE_USERMODE)
    // something something
    //
    return 123333;
}

