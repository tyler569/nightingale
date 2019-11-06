
#pragma once
#ifndef NG_MULTIBOOT2_H
#define NG_MULTIBOOT2_H

/*  multiboot2.h - Multiboot 2 header file.  */
/*  Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1
#include <basic.h>

/* How many bytes from the start of the file we search for the header.  */
#define MULTIBOOT_SEARCH 32768
#define MULTIBOOT_HEADER_ALIGN 8

/* The magic field should contain this.  */
#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

/* This should be in %eax.  */
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

/* Alignment of multiboot modules.  */
#define MULTIBOOT_MOD_ALIGN 0x00001000

/* Alignment of the multiboot info structure.  */
#define MULTIBOOT_INFO_ALIGN 0x00000008

/* Flags set in the 'flags' member of the multiboot header.  */

#define MULTIBOOT_TAG_ALIGN 8
#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_TYPE_CMDLINE 1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT_TAG_TYPE_MODULE 3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT_TAG_TYPE_BOOTDEV 5
#define MULTIBOOT_TAG_TYPE_MMAP 6
#define MULTIBOOT_TAG_TYPE_VBE 7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS 9
#define MULTIBOOT_TAG_TYPE_APM 10
#define MULTIBOOT_TAG_TYPE_EFI32 11
#define MULTIBOOT_TAG_TYPE_EFI64 12
#define MULTIBOOT_TAG_TYPE_SMBIOS 13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD 14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW 15
#define MULTIBOOT_TAG_TYPE_NETWORK 16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP 17
#define MULTIBOOT_TAG_TYPE_EFI_BS 18
#define MULTIBOOT_TAG_TYPE_EFI32_IH 19
#define MULTIBOOT_TAG_TYPE_EFI64_IH 20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21

#define MULTIBOOT_HEADER_TAG_END 0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_TAG_ADDRESS 2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN 6
#define MULTIBOOT_HEADER_TAG_EFI_BS 7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64 9
#define MULTIBOOT_HEADER_TAG_RELOCATABLE 10

#define MULTIBOOT_ARCHITECTURE_I386 0
#define MULTIBOOT_ARCHITECTURE_MIPS32 4
#define MULTIBOOT_HEADER_TAG_OPTIONAL 1

#define MULTIBOOT_LOAD_PREFERENCE_NONE 0
#define MULTIBOOT_LOAD_PREFERENCE_LOW 1
#define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

#define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED 1
#define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

#ifndef ASM_FILE

typedef struct multiboot_header {
        /* Must be MULTIBOOT_MAGIC - see above.  */
        uint32_t magic;

        /* ISA */
        uint32_t architecture;

        /* Total header length.  */
        uint32_t header_length;

        /* The above fields plus this one must equal 0 mod 2^32. */
        uint32_t checksum;
} multiboot_header;

typedef struct multiboot_header_tag {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
} multiboot_header_tag;

typedef struct multiboot_header_tag_information_request {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t requests[];
} multiboot_header_tag_information_request;

typedef struct multiboot_header_tag_address {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t header_addr;
        uint32_t load_addr;
        uint32_t load_end_addr;
        uint32_t bss_end_addr;
} multiboot_header_tag_address;

typedef struct multiboot_header_tag_entry_address {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t entry_addr;
} multiboot_header_tag_entry_address;

typedef struct multiboot_header_tag_console_flags {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t console_flags;
} multiboot_header_tag_console_flags;

typedef struct multiboot_header_tag_framebuffer {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
} multiboot_header_tag_framebuffer;

typedef struct multiboot_header_tag_module_align {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
} multiboot_header_tag_module_align;

typedef struct multiboot_header_tag_relocatable {
        uint16_t type;
        uint16_t flags;
        uint32_t size;
        uint32_t min_addr;
        uint32_t max_addr;
        uint32_t align;
        uint32_t preference;
} multiboot_header_tag_relocatable;

typedef struct multiboot_color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
} multiboot_color;

typedef struct multiboot_mmap_entry {
        uint64_t addr;
        uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5
        uint32_t type;
        uint32_t zero;
} multiboot_mmap_entry;
// typedef struct multiboot_mmap_entry multiboot_memory_map_t;

typedef struct multiboot_tag {
        uint32_t type;
        uint32_t size;
} multiboot_tag;

typedef struct multiboot_tag_string {
        uint32_t type;
        uint32_t size;
        char string[];
} multiboot_tag_string;

typedef struct multiboot_tag_module {
        uint32_t type;
        uint32_t size;
        uint32_t mod_start;
        uint32_t mod_end;
        char cmdline[];
} multiboot_tag_module;

typedef struct multiboot_tag_basic_meminfo {
        uint32_t type;
        uint32_t size;
        uint32_t mem_lower;
        uint32_t mem_upper;
} multiboot_tag_basic_meminfo;

typedef struct multiboot_tag_bootdev {
        uint32_t type;
        uint32_t size;
        uint32_t biosdev;
        uint32_t slice;
        uint32_t part;
} multiboot_tag_bootdev;

typedef struct multiboot_tag_mmap {
        uint32_t type;
        uint32_t size;
        uint32_t entry_size;
        uint32_t entry_version;
        multiboot_mmap_entry entries[];
} multiboot_tag_mmap;

typedef struct multiboot_vbe_info_block {
        uint8_t external_specification[512];
} multiboot_vbe_info_block;

typedef struct multiboot_vbe_mode_info_block {
        uint8_t external_specification[256];
} multiboot_vbe_mode_info_block;

typedef struct multiboot_tag_vbe {
        uint32_t type;
        uint32_t size;

        uint16_t vbe_mode;
        uint16_t vbe_interface_seg;
        uint16_t vbe_interface_off;
        uint16_t vbe_interface_len;

        multiboot_vbe_info_block vbe_control_info;
        multiboot_vbe_mode_info_block vbe_mode_info;
} multiboot_tag_vbe;

typedef struct multiboot_tag_framebuffer_common {
        uint32_t type;
        uint32_t size;

        uint64_t framebuffer_addr;
        uint32_t framebuffer_pitch;
        uint32_t framebuffer_width;
        uint32_t framebuffer_height;
        uint8_t framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
        uint8_t framebuffer_type;
        uint16_t reserved;
} multiboot_tag_framebuffer_common;

typedef struct multiboot_tag_framebuffer {
        multiboot_tag_framebuffer_common common;

        union {
                struct {
                        uint16_t framebuffer_palette_num_colors;
                        multiboot_color framebuffer_palette[1]; // not valid FAM
                };
                struct {
                        uint8_t framebuffer_red_field_position;
                        uint8_t framebuffer_red_mask_size;
                        uint8_t framebuffer_green_field_position;
                        uint8_t framebuffer_green_mask_size;
                        uint8_t framebuffer_blue_field_position;
                        uint8_t framebuffer_blue_mask_size;
                };
        };
} multiboot_tag_framebuffer;

typedef struct multiboot_tag_elf_sections {
        uint32_t type;
        uint32_t size;
        uint32_t num;
        uint32_t entsize;
        uint32_t shndx;
        char sections[];
} multiboot_tag_elf_sections;

typedef struct multiboot_tag_apm {
        uint32_t type;
        uint32_t size;
        uint16_t version;
        uint16_t cseg;
        uint32_t offset;
        uint16_t cseg_16;
        uint16_t dseg;
        uint16_t flags;
        uint16_t cseg_len;
        uint16_t cseg_16_len;
        uint16_t dseg_len;
} multiboot_tag_apm;

typedef struct multiboot_tag_efi32 {
        uint32_t type;
        uint32_t size;
        uint32_t pointer;
} multiboot_tag_efi32;

typedef struct multiboot_tag_efi64 {
        uint32_t type;
        uint32_t size;
        uint64_t pointer;
} multiboot_tag_efi64;

typedef struct multiboot_tag_smbios {
        uint32_t type;
        uint32_t size;
        uint8_t major;
        uint8_t minor;
        uint8_t reserved[6];
        uint8_t tables[];
} multiboot_tag_smbios;

typedef struct multiboot_tag_old_acpi {
        uint32_t type;
        uint32_t size;
        uint8_t rsdp[];
} multiboot_tag_old_acpi;

typedef struct multiboot_tag_new_acpi {
        uint32_t type;
        uint32_t size;
        uint8_t rsdp[];
} multiboot_tag_new_acpi;

typedef struct multiboot_tag_network {
        uint32_t type;
        uint32_t size;
        uint8_t dhcpack[];
} multiboot_tag_network;

typedef struct multiboot_tag_efi_mmap {
        uint32_t type;
        uint32_t size;
        uint32_t descr_size;
        uint32_t descr_vers;
        uint8_t efi_mmap[];
} multiboot_tag_efi_mmap;

typedef struct multiboot_tag_efi32_ih {
        uint32_t type;
        uint32_t size;
        uint32_t pointer;
} multiboot_tag_efi32_ih;

typedef struct multiboot_tag_efi64_ih {
        uint32_t type;
        uint32_t size;
        uint64_t pointer;
} multiboot_tag_efi64_ih;

typedef struct multiboot_tag_load_base_addr {
        uint32_t type;
        uint32_t size;
        uint32_t load_base_addr;
} multiboot_tag_load_base_addr;

#endif /* ! ASM_FILE */

#endif /* ! MULTIBOOT_HEADER */

#endif // NG_MULTIBOOT2_H

