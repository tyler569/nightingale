
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include "elf.h"

void print_elf64_header(struct elf64_header *header) {
    printf("elf64 header @%#lx\n", header);
    printf("  magic:    %.3s\n", ((char *)&header->magic) + 1);
    printf("  arch:     %i\n", header->arch);
    printf("  endian:   %i\n", header->endian);
    printf("  osabi:    %i\n", header->osabi);
    printf("\n");
    printf("  type:     %i\n", header->type);
    printf("  instr:    %i\n", header->instruction_set);
    printf("  version:  %i\n", header->version);
    printf("\n");
    printf("  entrypoint: %#lx\n", header->entry_point);
    printf("  prgm_hdr:   %#lx\n", header->program_header);
    printf("  sectn_hdr:  %#lx\n", header->section_header);
    printf("\n");
    printf("  flags:    %#x\n", header->flags);
    printf("  hdr_size: %i\n", header->header_size);
    printf("\n");
    printf("  prgm_hdr_ent_size: %i\n", header->program_header_ent_size);
    printf("  prgm_hdr_ent_cnt:  %i\n", header->program_header_ent_count);
    printf("  prgm_sec_ent_size: %i\n", header->section_header_ent_size);
    printf("  prgm_sec_ent_cnt:  %i\n", header->section_header_ent_count);
    printf("  sec_name_ix:  %i\n", header->section_name_index);
}

