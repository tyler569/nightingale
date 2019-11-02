
#pragma once
#ifndef NG_ELF_H
#define NG_ELF_H

#include <ng/basic.h>
#include <ng/multiboot2.h>
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

typedef struct {
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

#define SHT_NULL                0
#define SHT_PROGBITS            1
#define SHT_SYMTAB              2
#define SHT_STRTAB              3
#define SHT_RELA                4
#define SHT_HASH                5
#define SHT_DYNAMIC             6
#define SHT_NOTE                7
#define SHT_NOBITS              8
#define SHT_REL                 9
#define SHT_SHLIB              10
#define SHT_DYNSYM             11
#define SHT_LOPROC     0x70000000
#define SHT_HIPROC     0x7fffffff
#define SHT_LOUSER     0x80000000
#define SHT_HIUSER     0xffffffff

typedef struct {
        Elf64_Word st_name;
        unsigned char st_info;
        unsigned char st_other;
        Elf64_Half st_shndx;
        Elf64_Addr st_value;
        Elf64_Xword st_size;
} Elf64_Sym;

#define ELF_ST_BIND(i)       ((i) >> 4)
#define ELF_ST_TYPE(i)       ((i) & 0x0F)
// they're the same between 32 and 64
// #define ELF64_ST_INFO

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

#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2
#define STT_SECTION     3
#define STT_FILE        4
#define STT_LOPROC     13
#define STT_HIPROC     15

typedef struct {
        Elf32_Word st_name;
        Elf32_Addr st_value;
        Elf32_Word st_size;
        unsigned char st_info;
        unsigned char st_other;
        Elf32_Half st_shndx;
} Elf32_Sym;


typedef struct {
        Elf32_Addr r_offset;
        Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
        Elf32_Addr r_offset;
        Elf32_Word r_info;
        Elf32_Sword r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i)     ((i) >> 8)
#define ELF32_R_TYPE(i)    ((unsigned char)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)t)

typedef struct {
        Elf64_Addr r_offset;
        Elf64_Xword r_info;
} Elf64_Rel;

typedef struct {
        Elf64_Addr r_offset;
        Elf64_Xword r_info;
        Elf64_Sxword r_addend;
} Elf64_Rela;

#define ELF64_R_SYM(i)     ((i) >> 32)
#define ELF64_R_TYPE(i)    ((i) & 0xFFFFFFFF)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xFFFFFFFF))

#define R_386_NONE 0
#define R_386_32 1
#define R_386_PC32 2
#define R_386_GOT32 3
#define R_386_PLT32 4
#define R_386_COPY 5
#define R_386_GLOB_DAT 6
#define R_386_JMP_SLOT 7
#define R_386_RELATIVE 8
#define R_386_GOTOFF 9
#define R_386_GOTPC 10

                                      // param   calc
#define R_X86_64_NONE 0               // none    none
#define R_X86_64_64 1                 // word64  S + A
#define R_X86_64_PC32 2               // word32  S + A - P
#define R_X86_64_GOT32 3              // word32  G + A
#define R_X86_64_PLT32 4              // word32  L + A - P
#define R_X86_64_COPY 5               // none    none
#define R_X86_64_GLOB_DAT 6           // word64  S
#define R_X86_64_JUMP_SLOT 7          // word64  S
#define R_X86_64_RELATIVE 8           // word64  B + A
#define R_X86_64_GOTPCREL 9           // word32  G + GOT + A - P
#define R_X86_64_32 10                // word32  S + A
#define R_X86_64_32S 11               // word32  S + A
#define R_X86_64_16 12                // word16  S + A
#define R_X86_64_PC16 13              // word16  S + A - P
#define R_X86_64_8 14                 // word8   S + A
#define R_X86_64_PC8 15               // word8   S + A - P
#define R_X86_64_DPTMOD64 16          // word64   
#define R_X86_64_DTPOFF64 17          // word64   
#define R_X86_64_TPOFF64 18           // word64   
#define R_X86_64_TLSGD 19             // word32       
#define R_X86_64_TLSLD 20             // word32   
#define R_X86_64_DTPOFF32 21          // word32   
#define R_X86_64_GOTTPOFF 22          // word32   
#define R_X86_64_TPOFF32 23           // word32   


#if X86_64
typedef Elf64_Ehdr Elf;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym  Elf_Sym;
// typedef Elf64_Dyn  Elf_Dyn;
#elif I686
typedef Elf32_Ehdr Elf;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym  Elf_Sym;
// typedef Elf32_Dyn  Elf_Dyn;
#endif

struct elfinfo {
        Elf *elf;

        size_t shdr_count;
        size_t shstrndx;

        Elf_Shdr *shdr;
        Elf_Shdr *strtab;
        Elf_Shdr *symtab;
        char *shstrtab;
};

extern struct elfinfo ngk_elfinfo;

void *elf_at(Elf *elf, size_t offset);
int elf_verify(Elf *header);
int elf_load(Elf *header);
void elf_debugprint(Elf *elf);
//void elf_print_syms(Elf *elf);
struct elfinfo elf_info(Elf *elf);
void mb_elf_info(multiboot_tag_elf_sections* mb, struct elfinfo *ei);
size_t elf_get_sym_off(struct elfinfo *ei, const char *sym_name);
//void elf_print_rels(Elf *elf);
void elf_resolve_symbols(struct elfinfo *master, struct elfinfo *child);
//void elf_resolve_symbols_from_shdrs(Elf_Shdr *m_symtab, Elf_Shdr *m_strtab,
//                                    Elf *child);
int elf_relocate_object(struct elfinfo *ei, uintptr_t new_base);

void elf_find_symbol_by_addr(struct elfinfo *elf, uintptr_t addr, char *buf);

#endif // NG_ELF_H

