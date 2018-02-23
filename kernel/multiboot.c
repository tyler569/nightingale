
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

static char *command_line;
static char *bootloader_name;

static multiboot_mmap_entry *memory_map;
static usize memory_map_len;

static void *acpi_rsdp;

void mb_parse(usize mb_info) {
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
            // TODO
            break;
        case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        case MULTIBOOT_TAG_TYPE_ACPI_NEW:
            acpi_rsdp = (void *)tag;
            break;
        default:
            break;
        }
    }
}

void mb_mmap_print() {
    for (usize i=0; i<memory_map_len; i++) {
        printf("mmap: %#p + %lx type %i\n",
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

usize mb_mmap_something() {
/* saved for later
*/
}
