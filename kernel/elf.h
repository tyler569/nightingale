
#pragma once
#ifndef NIGHTINGALE_ELF_H
#define NIGHTINGALE_ELF_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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

typedef uintptr_t   Elf64_Addr;       // Unsigned program address
typedef size_t      Elf64_Off;        // Unsigned file offset
typedef uint16_t    Elf64_Section;    // Unsigned section index
typedef uint16_t    Elf64_Versym;     // Unsigned version symbol information
typedef uint8_t     Elf64_Byte;
typedef uint16_t    Elf64_Half;
typedef int32_t     Elf64_Sword;
typedef uint32_t    Elf64_Word;
typedef int64_t     Elf64_Sxword;
typedef uint64_t    Elf65_Xword;

typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    Elf64_Addr    e_entry;
    Elf64_Off     e_phoff;
    Elf64_Off     e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf64_Ehdr;

/* p_type values: */
#define PT_NULL     0   // unused entry
#define PT_LOAD     1   // loadable segment
#define PT_DYNAMIC  2   // dynamic linking info segment
#define PT_INTERP   3   // pathname of interpreter
#define PT_NOTE     4   // auxiliary information
#define PT_SHLIB    5   // reserved
#define PT_PHDR     6   // the program header itself
#define PT_TLS      7   // thread local storage

/* p_flags values: */
#define PF_X        1   // executable
#define PF_W        2   // writeable
#define PF_R        4   // readable 

typedef struct {
    uint32_t   p_type;
    uint32_t   p_flags;
    Elf64_Off  p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    uint64_t   p_filesz;
    uint64_t   p_memsz;
    uint64_t   p_align;
} Elf64_Phdr;

bool check_elf(Elf64_Ehdr *header);
int load_elf(Elf64_Ehdr *elf);
void print_elf(Elf64_Ehdr *elf);

#endif
