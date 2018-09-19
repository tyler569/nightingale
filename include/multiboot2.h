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
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1
#include <basic.h>

/* How many bytes from the start of the file we search for the header.  */
#define MULTIBOOT_SEARCH                    32768
#define MULTIBOOT_HEADER_ALIGN              8

/* The magic field should contain this.  */
#define MULTIBOOT2_HEADER_MAGIC             0xe85250d6

/* This should be in %eax.  */
#define MULTIBOOT2_BOOTLOADER_MAGIC         0x36d76289

/* Alignment of multiboot modules.  */
#define MULTIBOOT_MOD_ALIGN                 0x00001000

/* Alignment of the multiboot info structure.  */
#define MULTIBOOT_INFO_ALIGN                0x00000008

/* Flags set in the 'flags' member of the multiboot header.  */

#define MULTIBOOT_TAG_ALIGN                  8
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10
#define MULTIBOOT_TAG_TYPE_EFI32             11
#define MULTIBOOT_TAG_TYPE_EFI64             12
#define MULTIBOOT_TAG_TYPE_SMBIOS            13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
#define MULTIBOOT_TAG_TYPE_NETWORK           16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
#define MULTIBOOT_TAG_TYPE_EFI_BS            18
#define MULTIBOOT_TAG_TYPE_EFI32_IH          19
#define MULTIBOOT_TAG_TYPE_EFI64_IH          20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

#define MULTIBOOT_HEADER_TAG_END  0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST  1
#define MULTIBOOT_HEADER_TAG_ADDRESS  2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS  3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS  4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER  5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN  6
#define MULTIBOOT_HEADER_TAG_EFI_BS  7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64  9
#define MULTIBOOT_HEADER_TAG_RELOCATABLE  10

#define MULTIBOOT_ARCHITECTURE_I386  0
#define MULTIBOOT_ARCHITECTURE_MIPS32  4
#define MULTIBOOT_HEADER_TAG_OPTIONAL 1

#define MULTIBOOT_LOAD_PREFERENCE_NONE 0
#define MULTIBOOT_LOAD_PREFERENCE_LOW 1
#define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

#define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED 1
#define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

#ifndef ASM_FILE

typedef struct multiboot_header {
    /* Must be MULTIBOOT_MAGIC - see above.  */
    u32 magic;

    /* ISA */
    u32 architecture;

    /* Total header length.  */
    u32 header_length;

    /* The above fields plus this one must equal 0 mod 2^32. */
    u32 checksum;
} multiboot_header;

typedef struct multiboot_header_tag {
    u16 type;
    u16 flags;
    u32 size;
} multiboot_header_tag;

typedef struct multiboot_header_tag_information_request {
    u16 type;
    u16 flags;
    u32 size;
    u32 requests[];
} multiboot_header_tag_information_request;

typedef struct multiboot_header_tag_address {
    u16 type;
    u16 flags;
    u32 size;
    u32 header_addr;
    u32 load_addr;
    u32 load_end_addr;
    u32 bss_end_addr;
} multiboot_header_tag_address;

typedef struct multiboot_header_tag_entry_address {
    u16 type;
    u16 flags;
    u32 size;
    u32 entry_addr;
} multiboot_header_tag_entry_address;

typedef struct multiboot_header_tag_console_flags {
    u16 type;
    u16 flags;
    u32 size;
    u32 console_flags;
} multiboot_header_tag_console_flags;

typedef struct multiboot_header_tag_framebuffer {
    u16 type;
    u16 flags;
    u32 size;
    u32 width;
    u32 height;
    u32 depth;
} multiboot_header_tag_framebuffer;

typedef struct multiboot_header_tag_module_align {
    u16 type;
    u16 flags;
    u32 size;
} multiboot_header_tag_module_align;

typedef struct multiboot_header_tag_relocatable {
    u16 type;
    u16 flags;
    u32 size;
    u32 min_addr;
    u32 max_addr;
    u32 align;
    u32 preference;
} multiboot_header_tag_relocatable;

typedef struct multiboot_color {
    u8 red;
    u8 green;
    u8 blue;
} multiboot_color;

typedef struct multiboot_mmap_entry {
    u64 addr;
    u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE		1
#define MULTIBOOT_MEMORY_RESERVED		2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    u32 type;
    u32 zero;
} multiboot_mmap_entry;
//typedef struct multiboot_mmap_entry multiboot_memory_map_t;

typedef struct multiboot_tag {
    u32 type;
    u32 size;
} multiboot_tag;

typedef struct multiboot_tag_string {
    u32 type;
    u32 size;
    char string[];
} multiboot_tag_string;

typedef struct multiboot_tag_module {
    u32 type;
    u32 size;
    u32 mod_start;
    u32 mod_end;
    char cmdline[];
} multiboot_tag_module;

typedef struct multiboot_tag_basic_meminfo {
    u32 type;
    u32 size;
    u32 mem_lower;
    u32 mem_upper;
} multiboot_tag_basic_meminfo;

typedef struct multiboot_tag_bootdev {
    u32 type;
    u32 size;
    u32 biosdev;
    u32 slice;
    u32 part;
} multiboot_tag_bootdev;

typedef struct multiboot_tag_mmap {
    u32 type;
    u32 size;
    u32 entry_size;
    u32 entry_version;
    multiboot_mmap_entry entries[];  
} multiboot_tag_mmap;

typedef struct multiboot_vbe_info_block {
    u8 external_specification[512];
} multiboot_vbe_info_block;

typedef struct multiboot_vbe_mode_info_block {
    u8 external_specification[256];
} multiboot_vbe_mode_info_block;

typedef struct multiboot_tag_vbe {
    u32 type;
    u32 size;

    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;

    multiboot_vbe_info_block vbe_control_info;
    multiboot_vbe_mode_info_block vbe_mode_info;
} multiboot_tag_vbe;

typedef struct multiboot_tag_framebuffer_common {
    u32 type;
    u32 size;

    u64 framebuffer_addr;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u8 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT	2
    u8 framebuffer_type;
    u16 reserved;
} multiboot_tag_framebuffer_common;

typedef struct multiboot_tag_framebuffer {
    multiboot_tag_framebuffer_common common;

    union {
        struct {
            u16 framebuffer_palette_num_colors;
            multiboot_color framebuffer_palette[1]; // not valid FAM
        };
        struct {
            u8 framebuffer_red_field_position;
            u8 framebuffer_red_mask_size;
            u8 framebuffer_green_field_position;
            u8 framebuffer_green_mask_size;
            u8 framebuffer_blue_field_position;
            u8 framebuffer_blue_mask_size;
        };
    };
} multiboot_tag_framebuffer;

typedef struct multiboot_tag_elf_sections {
    u32 type;
    u32 size;
    u32 num;
    u32 entsize;
    u32 shndx;
    char sections[];
} multiboot_tag_elf_sections;

typedef struct multiboot_tag_apm {
    u32 type;
    u32 size;
    u16 version;
    u16 cseg;
    u32 offset;
    u16 cseg_16;
    u16 dseg;
    u16 flags;
    u16 cseg_len;
    u16 cseg_16_len;
    u16 dseg_len;
} multiboot_tag_apm;

typedef struct multiboot_tag_efi32 {
    u32 type;
    u32 size;
    u32 pointer;
} multiboot_tag_efi32;

typedef struct multiboot_tag_efi64 {
    u32 type;
    u32 size;
    u64 pointer;
} multiboot_tag_efi64;

typedef struct multiboot_tag_smbios {
    u32 type;
    u32 size;
    u8 major;
    u8 minor;
    u8 reserved[6];
    u8 tables[];
} multiboot_tag_smbios;

typedef struct multiboot_tag_old_acpi {
    u32 type;
    u32 size;
    u8 rsdp[];
} multiboot_tag_old_acpi;

typedef struct multiboot_tag_new_acpi {
    u32 type;
    u32 size;
    u8 rsdp[];
} multiboot_tag_new_acpi;

typedef struct multiboot_tag_network {
    u32 type;
    u32 size;
    u8 dhcpack[];
} multiboot_tag_network;

typedef struct multiboot_tag_efi_mmap {
    u32 type;
    u32 size;
    u32 descr_size;
    u32 descr_vers;
    u8 efi_mmap[];
} multiboot_tag_efi_mmap; 

typedef struct multiboot_tag_efi32_ih {
    u32 type;
    u32 size;
    u32 pointer;
} multiboot_tag_efi32_ih;

typedef struct multiboot_tag_efi64_ih {
    u32 type;
    u32 size;
    u64 pointer;
} multiboot_tag_efi64_ih;

typedef struct multiboot_tag_load_base_addr {
    u32 type;
    u32 size;
    u32 load_base_addr;
} multiboot_tag_load_base_addr;

#endif /* ! ASM_FILE */

#endif /* ! MULTIBOOT_HEADER */
