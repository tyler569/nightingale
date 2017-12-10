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

typedef u8		multiboot_u8;
typedef u16		multiboot_u16;
typedef u32		multiboot_u32;
typedef u64 	multiboot_u64;

struct multiboot_header
{
  /* Must be MULTIBOOT_MAGIC - see above.  */
  multiboot_u32 magic;

  /* ISA */
  multiboot_u32 architecture;

  /* Total header length.  */
  multiboot_u32 header_length;

  /* The above fields plus this one must equal 0 mod 2^32. */
  multiboot_u32 checksum;
};

struct multiboot_header_tag
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
};

struct multiboot_header_tag_information_request
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 requests[0];
};

struct multiboot_header_tag_address
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 header_addr;
  multiboot_u32 load_addr;
  multiboot_u32 load_end_addr;
  multiboot_u32 bss_end_addr;
};

struct multiboot_header_tag_entry_address
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 entry_addr;
};

struct multiboot_header_tag_console_flags
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 console_flags;
};

struct multiboot_header_tag_framebuffer
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 width;
  multiboot_u32 height;
  multiboot_u32 depth;
};

struct multiboot_header_tag_module_align
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
};

struct multiboot_header_tag_relocatable
{
  multiboot_u16 type;
  multiboot_u16 flags;
  multiboot_u32 size;
  multiboot_u32 min_addr;
  multiboot_u32 max_addr;
  multiboot_u32 align;
  multiboot_u32 preference;
};

struct multiboot_color
{
  multiboot_u8 red;
  multiboot_u8 green;
  multiboot_u8 blue;
};

struct multiboot_mmap_entry
{
  multiboot_u64 addr;
  multiboot_u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE		1
#define MULTIBOOT_MEMORY_RESERVED		2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  multiboot_u32 type;
  multiboot_u32 zero;
};
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct multiboot_tag
{
  multiboot_u32 type;
  multiboot_u32 size;
};

struct multiboot_tag_string
{
  multiboot_u32 type;
  multiboot_u32 size;
  char string[0];
};

struct multiboot_tag_module
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 mod_start;
  multiboot_u32 mod_end;
  char cmdline[0];
};

struct multiboot_tag_basic_meminfo
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 mem_lower;
  multiboot_u32 mem_upper;
};

struct multiboot_tag_bootdev
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 biosdev;
  multiboot_u32 slice;
  multiboot_u32 part;
};

struct multiboot_tag_mmap
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 entry_size;
  multiboot_u32 entry_version;
  struct multiboot_mmap_entry entries[0];  
};

struct multiboot_vbe_info_block
{
  multiboot_u8 external_specification[512];
};

struct multiboot_vbe_mode_info_block
{
  multiboot_u8 external_specification[256];
};

struct multiboot_tag_vbe
{
  multiboot_u32 type;
  multiboot_u32 size;

  multiboot_u16 vbe_mode;
  multiboot_u16 vbe_interface_seg;
  multiboot_u16 vbe_interface_off;
  multiboot_u16 vbe_interface_len;

  struct multiboot_vbe_info_block vbe_control_info;
  struct multiboot_vbe_mode_info_block vbe_mode_info;
};

struct multiboot_tag_framebuffer_common
{
  multiboot_u32 type;
  multiboot_u32 size;

  multiboot_u64 framebuffer_addr;
  multiboot_u32 framebuffer_pitch;
  multiboot_u32 framebuffer_width;
  multiboot_u32 framebuffer_height;
  multiboot_u8 framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT	2
  multiboot_u8 framebuffer_type;
  multiboot_u16 reserved;
};

struct multiboot_tag_framebuffer
{
  struct multiboot_tag_framebuffer_common common;

  union
  {
    struct
    {
      multiboot_u16 framebuffer_palette_num_colors;
      struct multiboot_color framebuffer_palette[0];
    };
    struct
    {
      multiboot_u8 framebuffer_red_field_position;
      multiboot_u8 framebuffer_red_mask_size;
      multiboot_u8 framebuffer_green_field_position;
      multiboot_u8 framebuffer_green_mask_size;
      multiboot_u8 framebuffer_blue_field_position;
      multiboot_u8 framebuffer_blue_mask_size;
    };
  };
};

struct multiboot_tag_elf_sections
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 num;
  multiboot_u32 entsize;
  multiboot_u32 shndx;
  char sections[0];
};

struct multiboot_tag_apm
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u16 version;
  multiboot_u16 cseg;
  multiboot_u32 offset;
  multiboot_u16 cseg_16;
  multiboot_u16 dseg;
  multiboot_u16 flags;
  multiboot_u16 cseg_len;
  multiboot_u16 cseg_16_len;
  multiboot_u16 dseg_len;
};

struct multiboot_tag_efi32
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 pointer;
};

struct multiboot_tag_efi64
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u64 pointer;
};

struct multiboot_tag_smbios
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u8 major;
  multiboot_u8 minor;
  multiboot_u8 reserved[6];
  multiboot_u8 tables[0];
};

struct multiboot_tag_old_acpi
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u8 rsdp[0];
};

struct multiboot_tag_new_acpi
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u8 rsdp[0];
};

struct multiboot_tag_network
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u8 dhcpack[0];
};

struct multiboot_tag_efi_mmap
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 descr_size;
  multiboot_u32 descr_vers;
  multiboot_u8 efi_mmap[0];
}; 

struct multiboot_tag_efi32_ih
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 pointer;
};

struct multiboot_tag_efi64_ih
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u64 pointer;
};

struct multiboot_tag_load_base_addr
{
  multiboot_u32 type;
  multiboot_u32 size;
  multiboot_u32 load_base_addr;
};

#endif /* ! ASM_FILE */

#endif /* ! MULTIBOOT_HEADER */
