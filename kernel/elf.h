
#pragma once
#ifndef NIGHTINGALE_ELF_H
#define NIGHTINGALE_ELF_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

#define ELF32 1
#define ELF64 2

#define ELFLE 1
#define ELFBE 2

#define ELFABI 0

#define ELFREL  1
#define ELFEXEC 2
#define ELFDYN  3


struct elf_header {
    uint8_t ident[16];

    uint16_t type;
    uint16_t machine;
    uint16_t version;
    
    uint64_t entrypoint;
    uint64_t program_header;
    uint64_t section_header;

    uint32_t flags;

    uint16_t header_size;
    uint16_t ph_ent_size;
    uint16_t ph_ent_count;
    uint16_t sh_ent_size;
    uint16_t sh_ent_count;
    uint16_t section_name_index;
};

int check_elf(struct elf_header *header);

#endif
