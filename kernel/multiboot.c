
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

static char *command_line;
static char *bootloader_name;
static multiboot_mmap_entry *memory_map;
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

usize mb_mmap_something() {
/* saved for later
    (u8 *)mmap < (u8 *)tag + tag->size;
    mmap = (multiboot_mmap_entry *)((unsigned long) mmap
        + ((struct multiboot_tag_mmap *) tag)->entry_size);

    printf("base: %p, len: %x (%iM), type %i\n",
        mmap->addr, mmap->len, mmap->len/(1024*1024), mmap->type);
*/
}
