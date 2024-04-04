#pragma once

#include <nx/type_traits.h>
#include <stddef.h>
#include <stdint.h>
// #include <ostream>

namespace nx::elf {

using ELF64_Addr = uint64_t;
using ELF64_Off = uint64_t;
using ELF64_Half = uint16_t;
using ELF64_Word = uint32_t;
using ELF64_Sword = int32_t;
using ELF64_Xword = uint64_t;
using ELF64_Sxword = int64_t;

struct ehdr {
    unsigned char e_ident[16];
    ELF64_Half e_type;
    ELF64_Half e_machine;
    ELF64_Word e_version;
    ELF64_Addr e_entry;
    ELF64_Off e_phoff;
    ELF64_Off e_shoff;
    ELF64_Word e_flags;
    ELF64_Half e_ehsize;
    ELF64_Half e_phentsize;
    ELF64_Half e_phnum;
    ELF64_Half e_shentsize;
    ELF64_Half e_shnum;
    ELF64_Half e_shstrndx;
};

struct phdr {
    ELF64_Word p_type;
    ELF64_Word p_flags;
    ELF64_Off p_offset;
    ELF64_Addr p_vaddr;
    ELF64_Addr p_paddr;
    ELF64_Xword p_filesz;
    ELF64_Xword p_memsz;
    ELF64_Xword p_align;
};

struct shdr {
    ELF64_Word sh_name;
    ELF64_Word sh_type;
    ELF64_Xword sh_flags;
    ELF64_Addr sh_addr;
    ELF64_Off sh_offset;
    ELF64_Xword sh_size;
    ELF64_Word sh_link;
    ELF64_Word sh_info;
    ELF64_Xword sh_addralign;
    ELF64_Xword sh_entsize;
};

struct sym {
    ELF64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    ELF64_Half st_shndx;
    ELF64_Addr st_value;
    ELF64_Xword st_size;
};

struct rela {
    ELF64_Addr r_offset;
    ELF64_Xword r_info;
    ELF64_Sxword r_addend;
};

struct dyn {
    ELF64_Sxword d_tag;
    union {
        ELF64_Xword d_val;
        ELF64_Addr d_ptr;
    } d_un;
};

struct dynsym {
    ELF64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    ELF64_Half st_shndx;
    ELF64_Addr st_value;
    ELF64_Xword st_size;
};

enum program_header_type : ELF64_Word {
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_TLS = 7,
};

enum section_header_type : ELF64_Word {
    SHT_NULL = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_HASH = 5,
    SHT_DYNAMIC = 6,
    SHT_NOTE = 7,
    SHT_NOBITS = 8,
    SHT_REL = 9,
    SHT_SHLIB = 10,
    SHT_DYNSYM = 11,
};

enum section_header_special : ELF64_Word {
    SHN_UNDEF = 0,
    SHN_LORESERVE = 0xff00,
    SHN_ABS = 0xfff1,
    SHN_COMMON = 0xfff2,
    SHN_HIRESERVE = 0xffff,
};

enum symbol_type : unsigned char {
    STT_NOTYPE = 0,
    STT_OBJECT = 1,
    STT_FUNC = 2,
    STT_SECTION = 3,
    STT_FILE = 4,
};

enum symbol_binding : unsigned char {
    STB_LOCAL = 0,
    STB_GLOBAL = 1,
    STB_WEAK = 2,
};

enum relocation_type_x86 : ELF64_Word {
    RELA_X86_64_NONE = 0,
    RELA_X86_64_64 = 1,
    RELA_X86_64_PC32 = 2,
    RELA_X86_64_GOT32 = 3,
    RELA_X86_64_PLT32 = 4,
    RELA_X86_64_COPY = 5,
    RELA_X86_64_GLOB_DAT = 6,
    RELA_X86_64_JUMP_SLOT = 7,
    RELA_X86_64_RELATIVE = 8,
    RELA_X86_64_GOTPCREL = 9,
    RELA_X86_64_32 = 10,
    RELA_X86_64_32S = 11,
    RELA_X86_64_16 = 12,
    RELA_X86_64_PC16 = 13,
    RELA_X86_64_8 = 14,
    RELA_X86_64_PC8 = 15,
    RELA_X86_64_DTPMOD64 = 16,
    RELA_X86_64_DTPOFF64 = 17,
    RELA_X86_64_TPOFF64 = 18,
    RELA_X86_64_TLSGD = 19,
    RELA_X86_64_TLSLD = 20,
    RELA_X86_64_DTPOFF32 = 21,
    RELA_X86_64_GOTTPOFF = 22,
    RELA_X86_64_TPOFF32 = 23,
    RELA_X86_64_PC64 = 24,
    RELA_X86_64_GOTOFF64 = 25,
    RELA_X86_64_GOTPC32 = 26,
    RELA_X86_64_GOT64 = 27,
    RELA_X86_64_GOTPCREL64 = 28,
    RELA_X86_64_GOTPC64 = 29,
    RELA_X86_64_GOTPLT64 = 30,
    RELA_X86_64_PLTOFF64 = 31,
    RELA_X86_64_SIZE32 = 32,
    RELA_X86_64_SIZE64 = 33,
    RELA_X86_64_GOTPC32_TLSDESC = 34,
    RELA_X86_64_TLSDESC_CALL = 35,
    RELA_X86_64_TLSDESC = 36,
    RELA_X86_64_IRELATIVE = 37,
};

static constexpr size_t elf64_r_sym(ELF64_Xword info) { return info >> 32; }

static constexpr size_t elf64_r_type(ELF64_Xword info)
{
    return info & 0xffffffff;
}

static constexpr size_t elf64_r_info(size_t sym, size_t type)
{
    return (sym << 32) | (type & 0xffffffff);
}

// std::ostream& operator<<(std::ostream& os, const ehdr& header);
// std::ostream& operator<<(std::ostream& os, const phdr& header);
// std::ostream& operator<<(std::ostream& os, const shdr& header);
// std::ostream& operator<<(std::ostream& os, const sym& header);
// std::ostream& operator<<(std::ostream& os, const rela& header);
// std::ostream& operator<<(std::ostream& os, const dyn& header);
// std::ostream& operator<<(std::ostream& os, const dynsym& header);

class file {
    void *data {};
    const ehdr *header {};
    const phdr *program_headers {};
    const shdr *section_headers {};
    const shdr *symbol_section {};
    const shdr *string_section {};
    const sym *symbols {};
    const rela *relocations {};
    const dyn *dynamic {};
    const dynsym *dynamic_symbols {};
    const char *string_table {};
    const char *dynamic_string_table {};
    const char *dynamic_symbol_string_table {};

public:
    template <class T, class = nx::enable_if<nx::is_pointer_v<T>>>
    T get(size_t offset) const
    {
        return reinterpret_cast<T>(
            reinterpret_cast<const uint8_t *>(header) + offset);
    }

    explicit file(const char *path);
    explicit file(void *data, size_t size);
    void init();

    [[nodiscard]] const shdr *section(size_t index) const;
    [[nodiscard]] const shdr *section(const char *name) const;
    [[nodiscard]] const phdr *program(size_t index) const;
    [[nodiscard]] const char *string(size_t index) const;
    [[nodiscard]] const sym *symbol(size_t index) const;
    [[nodiscard]] const sym *symbol(const char *name) const;
    [[nodiscard]] const sym *symbol_by_address(uintptr_t address) const;

    void perform_relocations();
};

}

