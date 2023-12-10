#include <elf2.h>
#include <nx/concepts.h>
#include <nx/print.h>
#include <string.h>

namespace nx::elf {

//
//   Initialization
//

file::file(void *data, [[maybe_unused]] size_t size)
    : data(data)
{
    init();
}

void file::init()
{
    header = static_cast<const ehdr *>(data);

    if (memcmp(header->e_ident, "\x{7f}ELF", 4) != 0) {
        nx::print("Not an ELF file\n");
        return;
    }

    if (header->e_phnum != 0) {
        program_headers = get<const phdr *>(header->e_phoff);
    }
    if (header->e_shnum != 0) {
        section_headers = get<const shdr *>(header->e_shoff);
    }
    if (header->e_shstrndx != SHN_UNDEF) {
        string_section = section(header->e_shstrndx);
        string_table
            = get<const char *>(section_headers[header->e_shstrndx].sh_offset);
    }
    if (section(".symtab")) {
        symbol_section = section(".symtab");
        symbols = get<const sym *>(symbol_section->sh_offset);
    }
    if (section(".rela.text")) {
        relocations = get<const rela *>(section(".rela.text")->sh_offset);
    }
    if (section(".dynamic")) {
        dynamic = get<const dyn *>(section(".dynamic")->sh_offset);
    }
    if (section(".dynsym")) {
        dynamic_symbols = get<const dynsym *>(section(".dynsym")->sh_offset);
    }
    if (section(".dynstr")) {
        dynamic_string_table = get<const char *>(section(".dynstr")->sh_offset);
    }
    if (section(".dynsym")) {
        dynamic_symbol_string_table
            = get<const char *>(section(".dynstr")->sh_offset);
    }
}

//
//   Accessors
//

const shdr *file::section(size_t index) const
{
    if (index >= header->e_shnum)
        return nullptr;
    return get<const shdr *>(header->e_shoff + index * header->e_shentsize);
}

const shdr *file::section(const char *name) const
{
    for (size_t i = 0; i < header->e_shnum; i++) {
        const shdr *hdr = section(i);
        if (strcmp(string(hdr->sh_name), name) == 0) {
            return hdr;
        }
    }
    return nullptr;
}

const phdr *file::program(size_t index) const
{
    if (index >= header->e_phnum)
        return nullptr;
    return get<const phdr *>(header->e_phoff + index * header->e_phentsize);
}

const char *file::string(size_t index) const
{
    if (index >= string_section->sh_size)
        return nullptr;
    return string_table + index;
}

const sym *file::symbol(size_t index) const
{
    if (index >= symbol_section->sh_size / symbol_section->sh_entsize)
        return nullptr;
    return get<const sym *>(
        symbol_section->sh_offset + index * symbol_section->sh_entsize);
}

const sym *file::symbol(const char *name) const
{
    for (size_t i = 0; i < symbol_section->sh_size / sizeof(sym); i++) {
        if (strcmp(string(symbols[i].st_name), name) == 0) {
            return symbols + i;
        }
    }
    return nullptr;
}

const sym *file::symbol_by_address(uintptr_t address) const
{
    const sym *best_match = nullptr;
    uintptr_t addr_match = 0;

    for (size_t i = 0; i < symbol_section->sh_size / sizeof(sym); i++) {
        const sym *sym = symbols + i;
        if (sym->st_name == 0)
            continue;
        if (sym->st_value > address)
            continue;

        if (sym->st_value > addr_match) {
            best_match = sym;
            addr_match = sym->st_value;
        }
    }

    return best_match;
}

//
//   Relocations
//

template <nx::integral T> T to_int(const char *ptr)
{
    return static_cast<T>(reinterpret_cast<uintptr_t>(ptr));
}

void file::perform_relocations()
{
    for (size_t i = 0; i < section_headers->sh_size / sizeof(rela); i++) {
        const rela *rel = relocations + i;
        const sym *sym = symbols + elf64_r_sym(rel->r_info);
        const char *sym_name = string(sym->st_name);
        const shdr *sec = section(elf64_r_sym(rel->r_info));
        const char *section_name = string(sec->sh_name);
        const char *section_data = get<const char *>(sec->sh_offset);
        const char *section_addr = get<const char *>(sec->sh_addr);
        const char *section_end = section_addr + sec->sh_size;

        switch (elf64_r_type(rel->r_info)) {
        case RELA_X86_64_NONE:
            break;
        case RELA_X86_64_64:
            *(uint64_t *)(section_data + rel->r_offset)
                = (uint64_t)(section_addr + sym->st_value + rel->r_addend);
            break;
        case RELA_X86_64_32:
            *(uint32_t *)(section_data + rel->r_offset) = to_int<uint32_t>(
                section_addr + sym->st_value + rel->r_addend);
            break;
        case RELA_X86_64_32S:
            *(int32_t *)(section_data + rel->r_offset)
                = to_int<int32_t>(section_addr + sym->st_value + rel->r_addend);
            break;
        case RELA_X86_64_PC32: // fallthrough
        case RELA_X86_64_PLT32:
            *(uint32_t *)(section_data + rel->r_offset)
                = (uint32_t)(section_addr + sym->st_value + rel->r_addend
                    - section_data - rel->r_offset);
            break;
        default:
            nx::print(
                "unsupported relocation type: %i\n", elf64_r_type(rel->r_info));
        }
    }
}

//
//   Output
//

/*
std::ostream& operator<<(std::ostream& os, const ehdr& header)
{
    os << "e_ident: " << header.e_ident[1] << header.e_ident[2] <<
header.e_ident[3] << "\n"; os << "e_type: " << header.e_type << "\n"; os <<
"e_machine: " << header.e_machine << "\n"; os << "e_version: " <<
header.e_version << "\n"; os << "e_entry: " << header.e_entry << "\n"; os <<
"e_phoff: " << header.e_phoff << "\n"; os << "e_shoff: " << header.e_shoff <<
"\n"; os << "e_flags: " << header.e_flags << "\n"; os << "e_ehsize: " <<
header.e_ehsize << "\n"; os << "e_phentsize: " << header.e_phentsize << "\n"; os
<< "e_phnum: " << header.e_phnum << "\n"; os << "e_shentsize: " <<
header.e_shentsize << "\n"; os << "e_shnum: " << header.e_shnum << "\n"; os <<
"e_shstrndx: " << header.e_shstrndx << "\n"; return os;
}

std::ostream& operator<<(std::ostream& os, const phdr& header)
{
    os << "p_type: " << header.p_type << "\n";
    os << "p_flags: " << header.p_flags << "\n";
    os << "p_offset: " << header.p_offset << "\n";
    os << "p_vaddr: " << header.p_vaddr << "\n";
    os << "p_paddr: " << header.p_paddr << "\n";
    os << "p_filesz: " << header.p_filesz << "\n";
    os << "p_memsz: " << header.p_memsz << "\n";
    os << "p_align: " << header.p_align << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const shdr& header)
{
    os << "sh_name: " << header.sh_name << "\n";
    os << "sh_type: " << header.sh_type << "\n";
    os << "sh_flags: " << header.sh_flags << "\n";
    os << "sh_addr: " << header.sh_addr << "\n";
    os << "sh_offset: " << header.sh_offset << "\n";
    os << "sh_size: " << header.sh_size << "\n";
    os << "sh_link: " << header.sh_link << "\n";
    os << "sh_info: " << header.sh_info << "\n";
    os << "sh_addralign: " << header.sh_addralign << "\n";
    os << "sh_entsize: " << header.sh_entsize << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const sym& header)
{
    os << "st_name: " << header.st_name << "\n";
    os << "st_info: " << header.st_info << "\n";
    os << "st_other: " << header.st_other << "\n";
    os << "st_shndx: " << header.st_shndx << "\n";
    os << "st_value: " << header.st_value << "\n";
    os << "st_size: " << header.st_size << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const rela& header)
{
    os << "r_offset: " << header.r_offset << "\n";
    os << "r_info: " << header.r_info << "\n";
    os << "r_addend: " << header.r_addend << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const dynsym& header)
{
    os << "st_name: " << header.st_name << "\n";
    os << "st_info: " << header.st_info << "\n";
    os << "st_other: " << header.st_other << "\n";
    os << "st_shndx: " << header.st_shndx << "\n";
    os << "st_value: " << header.st_value << "\n";
    os << "st_size: " << header.st_size << "\n";
    return os;
}
 */

} // namespace nx::elf
