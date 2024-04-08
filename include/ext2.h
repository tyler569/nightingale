#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct ext2_super_block {
	uint32_t s_inodes_count;
	uint32_t s_blocks_count;
	uint32_t s_r_blocks_count;
	uint32_t s_free_blocks_count;
	uint32_t s_free_inodes_count;
	uint32_t s_first_data_block;
	uint32_t s_log_block_size;
	uint32_t s_log_frag_size;
	uint32_t s_blocks_per_group;
	uint32_t s_frags_per_group;
	uint32_t s_inodes_per_group;
	uint32_t s_mtime;
	uint32_t s_wtime;
	uint16_t s_mnt_count;
	uint16_t s_max_mnt_count;
	uint16_t s_magic;
	uint16_t s_state;
	uint16_t s_errors;
	uint16_t s_minor_rev_level;
	uint32_t s_lastcheck;
	uint32_t s_checkinterval;
	uint32_t s_creator_os;
	uint32_t s_rev_level;
	uint16_t s_def_resuid;
	uint16_t s_def_resgid;
	uint32_t s_first_ino;
	uint16_t s_inode_size;
	uint16_t s_block_group_nr;
	uint32_t s_feature_compat;
	uint32_t s_feature_incompat;
	uint32_t s_feature_ro_compat;
	uint32_t s_uuid[4];
	uint8_t s_volume_name[16];
	uint8_t s_last_mounted[64];
	uint32_t s_algo_bitmap;
};

static_assert(sizeof(struct ext2_super_block) == 204);

struct ext2_block_group_descriptor {
	uint32_t bg_block_bitmap;
	uint32_t bg_inode_bitmap;
	uint32_t bg_inode_table;
	uint16_t bg_free_blocks_count;
	uint16_t bg_free_inodes_count;
	uint16_t bg_used_dirs_count;
	uint16_t bg_pad;
	uint32_t bg_reserved[3];
};

static_assert(sizeof(struct ext2_block_group_descriptor) == 32);

#define BG_PER_BLOCK (1024 / sizeof(struct ext2_block_group_descriptor))

struct ext2_inode {
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size;
	uint32_t i_atime;
	uint32_t i_ctime;
	uint32_t i_mtime;
	uint32_t i_dtime;
	uint16_t i_gid;
	uint16_t i_links_count;
	uint32_t i_blocks;
	uint32_t i_flags;
	uint32_t i_osd1;
	uint32_t i_block[15];
	uint32_t i_generation;
	uint32_t i_file_acl;
	uint32_t i_dir_acl;
	uint32_t i_faddr;
	uint32_t i_osd2[3];
};

enum ext2_inode_mode {
	EXT2_S_IFSOCK = 0xC000,
	EXT2_S_IFLNK = 0xA000,
	EXT2_S_IFREG = 0x8000,
	EXT2_S_IFBLK = 0x6000,
	EXT2_S_IFDIR = 0x4000,
	EXT2_S_IFCHR = 0x2000,
	EXT2_S_IFIFO = 0x1000,
};

static_assert(sizeof(struct ext2_inode) == 128);

struct ext2_dir_entry {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t name_len;
	uint8_t file_type;
	char name[];
};

enum ext2_file_type {
	EXT2_FT_UNKNOWN = 0,
	EXT2_FT_REG_FILE = 1,
	EXT2_FT_DIR = 2,
	EXT2_FT_CHRDEV = 3,
	EXT2_FT_BLKDEV = 4,
	EXT2_FT_FIFO = 5,
	EXT2_FT_SOCK = 6,
	EXT2_FT_SYMLINK = 7,
};

#ifdef __kernel__
void ext2_super_block_info(struct ext2_super_block *sb);
void ext2_block_group_descriptor_info(struct ext2_block_group_descriptor *bg);
void ext2_inode_info(struct ext2_inode *i);
void ext2_info(void);
#endif

END_DECLS

