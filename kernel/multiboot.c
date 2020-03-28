
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
#include <ng/elf.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <ng/print.h>
#include <ng/vmm.h>

uintptr_t mb_info;

void mb_init(uintptr_t mb) {
        mb_info = mb;
}


void *mb_find_tag_of_type(int tag_type) {
        uint32_t length = *(uint32_t *)mb_info;
        for (multiboot_tag *tag = (multiboot_tag *)(mb_info + 8);
             tag->type != MULTIBOOT_TAG_TYPE_END;
             tag = (multiboot_tag *)((char *)tag + ((tag->size + 7) & ~7))) {
                if (tag->type == tag_type) {
                        return tag;
                }
        }
        return NULL;
}

char *mb_cmdline() {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_CMDLINE);
        if (tag) {
                return ((multiboot_tag_string *)tag)->string;
        } else {
                return NULL;
        }
}

char *mb_bootloader() {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME);
        if (tag) {
                return ((struct multiboot_tag_string *)tag)->string;
        } else {
                return NULL;
        }
}

void *mb_elf_tag() {
        return mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
}

void *mb_acpi_rsdp() {
        void *tag_old = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_ACPI_OLD);
        void *tag_new = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_ACPI_NEW);

        if (tag_old) {
                return (void *)((multiboot_tag_old_acpi *)tag_old)->rsdp;
        } else if (tag_new) {
                return (void *)((multiboot_tag_new_acpi *)tag_new)->rsdp;
        } else {
                return NULL;
        }
}

struct initfs_info mb_initfs_info() {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_MODULE);
        if (!tag) {
                return (struct initfs_info){0};
        }

        // TODO:
        // This means I can only support one module, should probably think
        // more deeply about how I can support an initfs and other things.

        multiboot_tag_module *mod = tag;
        uintptr_t mod_start = mod->mod_start;
        uintptr_t mod_end = mod->mod_end;
        mod_start += VMM_VIRTUAL_OFFSET;
        mod_end += VMM_VIRTUAL_OFFSET;

        return (struct initfs_info){mod_start, mod_end};
}

void mb_mmap_print() {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_MMAP);
        if (!tag)  return;

        multiboot_tag_mmap *mmap = tag;

        multiboot_mmap_entry *memory_map = mmap->entries;
        size_t memory_map_len = mmap->size / sizeof(multiboot_mmap_entry);

        for (size_t i = 0; i < memory_map_len; i++) {
                printf("mmap: %16llx:%10llx type %i\n", memory_map[i].addr,
                       memory_map[i].len, memory_map[i].type);
        }
}

size_t mb_mmap_total_usable() {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_MMAP);
        if (!tag)  return 0;

        multiboot_tag_mmap *mmap = tag;

        multiboot_mmap_entry *memory_map = mmap->entries;
        size_t memory_map_len = mmap->size / sizeof(multiboot_mmap_entry);

        size_t total_memory = 0;

        for (size_t i = 0; i < memory_map_len; i++) {
                if (memory_map[i].type == 1)
                        total_memory += memory_map[i].len;
        }

        return total_memory;
}

void mb_mmap_enumerate(void (*cb)(uintptr_t, uintptr_t, int)) {
        void *tag = mb_find_tag_of_type(MULTIBOOT_TAG_TYPE_MMAP);
        if (!tag)  return;

        multiboot_tag_mmap *mmap = tag;

        multiboot_mmap_entry *memory_map = mmap->entries;
        size_t memory_map_len = mmap->size / sizeof(multiboot_mmap_entry);

        for (size_t i = 0; i < memory_map_len; i++) {
                cb(memory_map[i].addr, memory_map[i].len, memory_map[i].type);
        }
}
