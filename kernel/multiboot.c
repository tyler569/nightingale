
/*
 * Multiboot manipulation code
 *
 * Ideas on things it can be:
 * query into it for pointers to things like rsdp
 * ask questions about the memory map
 *  - mmap_end_of_range_with(0x100) -> 0x9c000
 *  - mmap_type_of(0xb8000) -> 2
 *  - mmap_total_usable_memory() -> 64MB more or less
 *  - etc.
 */

#include <basic.h>
#include <multiboot2.h>
#include <print.h>
#include <panic.h>

static char *command_line;
static char *bootloader_name;

static multiboot_mmap_entry *memory_map;
static usize memory_map_len;

static multiboot_tag_elf_sections *elf_tag;

static void *acpi_rsdp;

void mb_parse(usize mb_info) {
    printf("mb: parsing multiboot at %#lx\n", mb_info);
    for (multiboot_tag *tag = (multiboot_tag *)(mb_info+8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (multiboot_tag *)((u8 *)tag + ((tag->size+7) & ~7))) {

        // Cache all the things

        switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            command_line = ((multiboot_tag_string *)tag)->string;
            break;
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            bootloader_name = ((struct multiboot_tag_string *) tag)->string;
            break;
        case MULTIBOOT_TAG_TYPE_MMAP:
            memory_map = ((multiboot_tag_mmap *)tag)->entries;
            memory_map_len = tag->size / sizeof(multiboot_mmap_entry);
            break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            elf_tag = tag;
            break;
        case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        case MULTIBOOT_TAG_TYPE_ACPI_NEW:
            acpi_rsdp = (void *)((multiboot_tag_old_acpi *)tag)->rsdp;
            break;
        default:
            printf("multiboot: unknown tag type %i encountered\n", tag->type);
            break;
        }
    }
}

void mb_mmap_print() {
    for (usize i=0; i<memory_map_len; i++) {
        printf("mmap: %16lx:%10lx type %i\n",
            memory_map[i].addr, memory_map[i].len, memory_map[i].type);
    }
}

usize mb_mmap_total_usable() {
    usize total_memory = 0;

    for (usize i=0; i<memory_map_len; i++) {
        if (memory_map[i].type == 1)  total_memory += memory_map[i].len;
    }

    return total_memory;
}

void mb_elf_print() {
    if (!elf_tag)  panic("Multiboot not parsed yet!");

    usize size = elf_tag->size;
    usize per = elf_tag->entsize;

    printf("elf: %lu sections at %lu per\n", size / per, per);
}

void *mb_elf_get() {
    return &elf_tag->sections;
}

void *mb_acpi_get_rsdp() {
    return acpi_rsdp;
}

