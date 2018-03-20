
#pragma once
#ifndef NIGHTINGALE_ELF_H
#define NIGHTINGALE_ELF_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

#define ELF_MAGIC "\x7FELF"

#define ELF_ARCH_32 1
#define ELF_ARCH_64 2

#define ELF_LE 1
#define ELF_BE 2

#define ELF_RELOCATABLE 1
#define ELF_EXECUTABLE 2
#define ELF_SHAREDLIB 3
#define ELF_CORE 4

#define ELF_INSTRUCTIONSET_NONE 0
#define ELF_INSTRUCTIONSET_x86 2
#define ELF_INSTRUCTIONSET_x86_64 0x3E

#define ELF_FLAG_NULL 0

struct __packed elf64_header {
    uint8_t magic[4];

    uint8_t arch;
    uint8_t endian;
    uint8_t version;
    uint8_t osabi;

    uint8_t _padding[8];

    uint16_t type;
    uint16_t instruction_set;
    uint16_t also_version;
    
    uint64_t entry_point;
    uint64_t program_header;
    uint64_t section_header;

    uint32_t flags;

    uint16_t header_size;
    uint16_t program_header_ent_size;
    uint16_t program_header_ent_count;
    uint16_t section_header_ent_size;
    uint16_t section_header_ent_count;
    uint16_t section_name_index;
};

void print_elf64_header(struct elf64_header *header);

#endif
