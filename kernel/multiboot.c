
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

#include <ng/basic.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <ng/print.h>
#include <ng/vmm.h>

// TODO: proper types in here

static char *command_line;
static char *bootloader_name;

static multiboot_mmap_entry *memory_map;
static size_t memory_map_len;

static multiboot_tag_elf_sections *elf_tag;

static void *acpi_rsdp;

static void *initfs;
static void *initfs_end;

void mb_parse(uintptr_t mb_info) {
        printf("mb: parsing multiboot at %#zx\n", mb_info);
        for (multiboot_tag *tag = (multiboot_tag *)(mb_info + 8);
             tag->type != MULTIBOOT_TAG_TYPE_END;
             tag = (multiboot_tag *)((char *)tag + ((tag->size + 7) & ~7))) {

                // Cache all the things

                switch (tag->type) {
                case MULTIBOOT_TAG_TYPE_CMDLINE:
                        command_line = ((multiboot_tag_string *)tag)->string;
                        printf("mb: kernel command line: %s\n", command_line);
                        break;
                case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                        bootloader_name =
                            ((struct multiboot_tag_string *)tag)->string;
                        printf("mb: bootloader name: %s\n", bootloader_name);
                        break;
                case MULTIBOOT_TAG_TYPE_MMAP:
                        memory_map = ((multiboot_tag_mmap *)tag)->entries;
                        memory_map_len =
                            tag->size / sizeof(multiboot_mmap_entry);
                        break;
                case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                        elf_tag = (void *)tag;
                        break;
                case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                        acpi_rsdp =
                            (void *)((multiboot_tag_old_acpi *)tag)->rsdp;
                        break;
                case MULTIBOOT_TAG_TYPE_MODULE:;
                        multiboot_tag_module *mod = (void *)tag;
                        initfs = (void *)((uintptr_t)mod->mod_start +
                                          VMM_VIRTUAL_OFFSET);
                        initfs_end = (void *)((uintptr_t)mod->mod_end +
                                              VMM_VIRTUAL_OFFSET);
                        printf("mb: initfs at %#zx\n", initfs);
                        break;
                default:
                        printf("mb: unknown tag type %i encountered\n",
                               tag->type);
                        break;
                }
        }
}

void mb_mmap_print() {
        for (size_t i = 0; i < memory_map_len; i++) {
                printf("mmap: %16llx:%10llx type %i\n", memory_map[i].addr,
                       memory_map[i].len, memory_map[i].type);
        }
}

void mb_pmm_mmap_alloc() {
        // TODO
}

size_t mb_mmap_total_usable() {
        size_t total_memory = 0;

        for (size_t i = 0; i < memory_map_len; i++) {
                if (memory_map[i].type == 1)
                        total_memory += memory_map[i].len;
        }

        return total_memory;
}

void mb_elf_print() {
        if (!elf_tag)
                panic("Multiboot not parsed yet!");

        size_t size = elf_tag->size;
        size_t per = elf_tag->entsize;

        printf("elf: %lu sections at %lu per\n", size / per, per);
}

void *mb_elf_get() { return &elf_tag->sections; }

void *mb_acpi_get_rsdp() { return acpi_rsdp; }

void *mb_get_initfs() { return initfs; }

void *mb_get_initfs_end() { return initfs_end; }

void mb_mmap_enumerate(void (*cb)(uintptr_t, uintptr_t, int)) {
        for (size_t i = 0; i < memory_map_len; i++) {
                cb(memory_map[i].addr, memory_map[i].len, memory_map[i].type);
        }
}

const char *mb_cmdline() { return command_line; }
