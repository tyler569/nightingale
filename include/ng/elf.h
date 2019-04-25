
#pragma once
#ifndef NIGHTINGALE_ELF_H
#define NIGHTINGALE_ELF_H

#include <ng/basic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ELF32 1
#define ELF64 2

#define ELFLE 1
#define ELFBE 2

#define ELFABI 0

#define ELFVERSION 1

#define ELFREL 1
#define ELFEXEC 2
#define ELFDYN 3

typedef uintptr_t Elf64_Addr;   // Unsigned program address
typedef size_t Elf64_Off;       // Unsigned file offset
typedef uint16_t Elf64_Section; // Unsigned section index
typedef uint16_t Elf64_Versym;  // Unsigned version symbol information
typedef uint8_t Elf64_Byte;
typedef uint16_t Elf64_Half;
typedef int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef int64_t Elf64_Sxword;
typedef uint64_t Elf64_Xword;

typedef struct {
        unsigned char e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        Elf64_Addr e_entry;
        Elf64_Off e_phoff;
        Elf64_Off e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef uintptr_t Elf32_Addr;   // Unsigned program address
typedef size_t Elf32_Off;       // Unsigned file offset
typedef uint16_t Elf32_Section; // Unsigned section index
typedef uint16_t Elf32_Versym;  // Unsigned version symbol information
typedef uint8_t Elf32_Byte;
typedef uint16_t Elf32_Half;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;
typedef int64_t Elf32_Sxword;
typedef uint64_t Elf32_Xword;

typedef struct {
        unsigned char e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        Elf32_Addr e_entry;
        Elf32_Off e_phoff;
        Elf32_Off e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
} Elf32_Ehdr;

/* p_type values: */
#define PT_NULL 0    // unused entry
#define PT_LOAD 1    // loadable segment
#define PT_DYNAMIC 2 // dynamic linking info segment
#define PT_INTERP 3  // pathname of interpreter
#define PT_NOTE 4    // auxiliary information
#define PT_SHLIB 5   // reserved
#define PT_PHDR 6    // the program header itself
#define PT_TLS 7     // thread local storage

/* p_flags values: */
#define PF_X 1 // executable
#define PF_W 2 // writeable
#define PF_R 4 // readable

typedef struct {
        uint32_t p_type;
        uint32_t p_flags;
        Elf64_Off p_offset;
        Elf64_Addr p_vaddr;
        Elf64_Addr p_paddr;
        uint64_t p_filesz;
        uint64_t p_memsz;
        uint64_t p_align;
} Elf64_Phdr;

typedef struct {
        uint32_t p_type;
        Elf32_Off p_offset;
        Elf32_Addr p_vaddr;
        Elf32_Addr p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
} Elf32_Phdr;

/* 88888888888888888888888888 */
typedef struct{
        Elf64_Word sh_name;
        Elf64_Word sh_type;
        Elf64_Xword sh_flags;
        Elf64_Addr sh_addr;
        Elf64_Off sh_offset;
        Elf64_Xword sh_size;
        Elf64_Word sh_link;
        Elf64_Word sh_info;
        Elf64_Xword sh_addralign;
        Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct {
        Elf64_Word st_name;
        unsigned char st_info;
        unsigned char st_other;
        Elf64_Half st_shndx;
        Elf64_Addr st_value;
        Elf64_Xword st_size;
} Elf64_Sym;
/* 88888888888888888888888888 */

typedef struct {
        Elf32_Word sh_name;
        Elf32_Word sh_type;
        Elf32_Word sh_flags;
        Elf32_Addr sh_addr;
        Elf32_Off sh_offset;
        Elf32_Word sh_size;
        Elf32_Word sh_link;
        Elf32_Word sh_info;
        Elf32_Word sh_addralign;
        Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
        Elf32_Word st_name;
        Elf32_Addr st_value;
        Elf32_Word st_size;
        unsigned char st_info;
        unsigned char st_other;
        Elf32_Half st_shndx;
} Elf32_Sym;


#if X86_64
typedef Elf64_Ehdr Elf;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym  Elf_Sym;
#elif I686
typedef Elf32_Ehdr Elf;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym  Elf_Sym;
#endif


int elf_verify(Elf *header);
int elf_load(Elf *elf);
void elf_debugprint(Elf *elf);
void elf_print_syms(Elf *elf);
void *elf_get_sym(const char *sym_name, Elf *elf);



#endif
