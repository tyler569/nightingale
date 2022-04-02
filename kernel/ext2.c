#include <basic.h>
#include <stdio.h>

struct super_block {
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
};

void super_block_info(struct super_block *sb)
{
    printf("super_block {\n");
    printf("\ts_inodes_count: %i\n", sb->s_inodes_count);
    printf("\ts_blocks_count: %i\n", sb->s_blocks_count);
    printf("\ts_r_blocks_count: %i\n", sb->s_r_blocks_count);
    printf("\ts_free_blocks_count: %i\n", sb->s_free_blocks_count);
    printf("\ts_free_inodes_count: %i\n", sb->s_free_inodes_count);
    printf("\ts_first_data_block: %i\n", sb->s_first_data_block);
    printf("\ts_log_block_size: %i\n", sb->s_log_block_size);
    printf("\ts_log_frag_size: %i\n", sb->s_log_frag_size);
    printf("\ts_blocks_per_group: %i\n", sb->s_blocks_per_group);
    printf("\ts_frags_per_group: %i\n", sb->s_frags_per_group);
    printf("\ts_inodes_per_group: %i\n", sb->s_inodes_per_group);
    printf("\ts_mtime: %i\n", sb->s_mtime);
    printf("\ts_wtime: %i\n", sb->s_wtime);
    printf("\ts_mnt_count: %i\n", sb->s_mnt_count);
    printf("\ts_max_mnt_count: %i\n", sb->s_max_mnt_count);
    printf("\ts_magic: %i\n", sb->s_magic);
    printf("\ts_state: %i\n", sb->s_state);
    printf("\ts_errors: %i\n", sb->s_errors);
    printf("\ts_minor_rev_level: %i\n", sb->s_minor_rev_level);
    printf("\ts_lastcheck: %i\n", sb->s_lastcheck);
    printf("\ts_checkinterval: %i\n", sb->s_checkinterval);
    printf("\ts_creator_os: %i\n", sb->s_creator_os);
    printf("\ts_rev_level: %i\n", sb->s_rev_level);
    printf("\ts_def_resuid: %i\n", sb->s_def_resuid);
    printf("\ts_def_resgid: %i\n", sb->s_def_resgid);
    printf("}\n");
}

struct block_group_descriptor {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
};

void block_group_descriptor_info(struct block_group_descriptor *bg)
{
    printf("block_group_descriptor {\n");
    printf("\tbg_block_bitmap: %i\n", bg->bg_block_bitmap);
    printf("\tbg_inode_bitmap: %i\n", bg->bg_inode_bitmap);
    printf("\tbg_inode_table: %i\n", bg->bg_inode_table);
    printf("\tbg_free_blocks_count: %i\n", bg->bg_free_blocks_count);
    printf("\tbg_free_inodes_count: %i\n", bg->bg_free_inodes_count);
    printf("\tbg_used_dirs_count: %i\n", bg->bg_used_dirs_count);
    printf("}\n");
}

struct inode {
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

void inode_info(struct inode *i)
{
    printf("inode {\n");
    printf("\ti_mode: %i\n", i->i_mode);
    printf("\ti_uid: %i\n", i->i_uid);
    printf("\ti_size: %i\n", i->i_size);
    printf("\ti_atime: %i\n", i->i_atime);
    printf("\ti_ctime: %i\n", i->i_ctime);
    printf("\ti_mtime: %i\n", i->i_mtime);
    printf("\ti_dtime: %i\n", i->i_dtime);
    printf("\ti_gid: %i\n", i->i_gid);
    printf("\ti_links_count: %i\n", i->i_links_count);
    printf("\ti_blocks: %i\n", i->i_blocks);
    printf("\ti_flags: %i\n", i->i_flags);
    printf("\ti_osd1: %i\n", i->i_osd1);
    // printf("\ti_block[15]: %i\n", i->i_block[15]);
    printf("\ti_block: [15 block pointers]\n");
    printf("\ti_generation: %i\n", i->i_generation);
    printf("\ti_file_acl: %i\n", i->i_file_acl);
    printf("\ti_dir_acl: %i\n", i->i_dir_acl);
    printf("\ti_faddr: %i\n", i->i_faddr);
    printf("\ti_osd2[0]: %i\n", i->i_osd2[0]);
    printf("\ti_osd2[1]: %i\n", i->i_osd2[1]);
    printf("\ti_osd2[2]: %i\n", i->i_osd2[2]);
    printf("}\n");
}

void read_block(long block_num, void *buffer)
{
    int read_sector(long, void *);

    read_sector(block_num * 2, buffer);
    read_sector(block_num * 2 + 1, buffer + 512);
}

void ext2_info(void)
{
    char buffer[1024];
    read_block(1, buffer);
    struct super_block sb = *(struct super_block *)buffer;

    super_block_info(&sb);

    int n_bgs = round_up(sb.s_blocks_count, sb.s_blocks_per_group)
        / sb.s_blocks_per_group;

    printf("block groups: %i\n", n_bgs);
    read_block(2, buffer);
    struct block_group_descriptor *pbg = (void *)buffer;
    for (int i = 0; i < n_bgs; i++) {
        struct block_group_descriptor bg = *pbg;
        block_group_descriptor_info(&bg);

        read_block(bg.bg_inode_table, buffer);
        struct inode *i_table = (struct inode *)buffer;
        for (int j = 0; j < 1024 / sizeof(struct inode); j++) {
            if (i_table[j].i_mode)
                inode_info(&i_table[j]);
        }

        pbg++;
    }

    printf("inode size: %lu\n", sizeof(struct inode));
}
