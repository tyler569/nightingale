#include <basic.h>
#include <ext2.h>
#include <stdio.h>

void ext2_super_block_info(struct ext2_super_block *sb)
{
    printf("ext2_super_block {\n");
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
    printf("\ts_first_ino: %i\n", sb->s_first_ino);
    printf("\ts_inode_size: %i\n", sb->s_inode_size);
    printf("\ts_block_group_nr: %i\n", sb->s_block_group_nr);
    printf("\ts_feature_compat: %i\n", sb->s_feature_compat);
    printf("\ts_feature_incompat: %i\n", sb->s_feature_incompat);
    printf("\ts_feature_ro_compat: %i\n", sb->s_feature_ro_compat);
    printf("\ts_uuid[0]: %i\n", sb->s_uuid[0]);
    printf("\ts_uuid[1]: %i\n", sb->s_uuid[1]);
    printf("\ts_uuid[2]: %i\n", sb->s_uuid[2]);
    printf("\ts_uuid[3]: %i\n", sb->s_uuid[3]);
    printf("\ts_volume_name: %s\n", sb->s_volume_name);
    printf("\ts_last_mounted: %s\n", sb->s_last_mounted);
    printf("\ts_algo_bitmap: %#x\n", sb->s_algo_bitmap);
    printf("}\n");
}

void ext2_block_group_descriptor_info(struct ext2_block_group_descriptor *bg)
{
    printf("ext2_block_group_descriptor {\n");
    printf("\tbg_block_bitmap: %i\n", bg->bg_block_bitmap);
    printf("\tbg_inode_bitmap: %i\n", bg->bg_inode_bitmap);
    printf("\tbg_inode_table: %i\n", bg->bg_inode_table);
    printf("\tbg_free_blocks_count: %i\n", bg->bg_free_blocks_count);
    printf("\tbg_free_inodes_count: %i\n", bg->bg_free_inodes_count);
    printf("\tbg_used_dirs_count: %i\n", bg->bg_used_dirs_count);
    printf("}\n");
}

void inode_info(struct ext2_inode *i)
{
    printf("ext2_inode {\n");
    printf("\ti_mode: %#x\n", i->i_mode);
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

struct ext2_block_group_descriptor get_bg(
    struct ext2_super_block *sb, long bg_number)
{
    long block_index = bg_number / BG_PER_BLOCK;
    long block_offset = bg_number % BG_PER_BLOCK;
    struct ext2_block_group_descriptor bgs[BG_PER_BLOCK];
    read_block(2 + block_index, bgs);

    return bgs[block_offset];
}

bool inode_debug = false;

struct ext2_inode get_inode(struct ext2_super_block *sb, long inode_number)
{
    // assert(inode_number > 0);

    if (inode_debug)
        printf("ext2_inode: %li ", inode_number);
    inode_number -= 1;

    long bg_number = inode_number / sb->s_inodes_per_group;
    struct ext2_block_group_descriptor bg = get_bg(sb, bg_number);
    long local_inode_index = inode_number % sb->s_inodes_per_group;

    if (inode_debug)
        printf("bg: %li table_block: %i ", bg_number, bg.bg_inode_table);

    long inode_per_block = 1024 / sb->s_inode_size;

    long block_index = local_inode_index / inode_per_block;
    long block_offset = local_inode_index % inode_per_block;

    if (inode_debug)
        printf("block: %li offset: %li\n", block_index, block_offset);

    char buffer[1024];
    read_block(bg.bg_inode_table + block_index, buffer);

    struct ext2_inode *i = PTR_ADD(buffer, block_offset * sb->s_inode_size);
    return *i;
}

void read_data(struct ext2_super_block *sb, struct ext2_inode *in, void *buffer,
    size_t len, size_t offset)
{
    // assert offset is a multiple of block_size

    size_t file_size = in->i_blocks * 512;
    size_t read_size = min(file_size, len);
    size_t blocks = read_size / 1024;
    if (blocks > 12) {
        printf("indirect blocks are not yet supported\n");
        return;
    }
    for (int i = 0; i < blocks; i++) {
        read_block(in->i_block[i], buffer + 1024 * i);
    }
}

void read_dir(struct ext2_super_block *sb, struct ext2_inode *dir)
{
    char buffer[1024];
    read_data(sb, dir, buffer, 1024, 0);
    struct ext2_dir_entry *d = (void *)buffer;

    size_t offset = 0;
    while (d->inode && offset < dir->i_size) {
        printf("\"%.*s\" -> %i\n", d->name_len, d->name, d->inode);
        offset += d->rec_len;
        d = PTR_ADD(d, d->rec_len);
    }
}

void tree_dir(struct ext2_super_block *sb, struct ext2_inode *dir)
{
    char buffer[1024];
    read_data(sb, dir, buffer, 1024, 0);
    struct ext2_dir_entry *d = (void *)buffer;

    size_t offset = 0;
    while (d->inode && offset < dir->i_size) {
        printf("\"%.*s\" -> %i\n", d->name_len, d->name, d->inode);
        if (d->name[0] != '.' && d->file_type == EXT2_FT_DIR) {
            struct ext2_inode subdir = get_inode(sb, d->inode);
            tree_dir(sb, &subdir);
        }
        offset += d->rec_len;
        d = PTR_ADD(d, d->rec_len);
    }
}

void info(struct ext2_super_block *sb, int id)
{
    struct ext2_inode maybe_root = get_inode(sb, id);
    inode_info(&maybe_root);
}

void numbers(struct ext2_super_block *sb)
{
    long block_size = 1024 << sb->s_log_block_size;
    printf("block_size:  %li\n", block_size);
    printf("i blocks:    %li\n", block_size * 12);
    long pointers_per_block = block_size / sizeof(uint32_t);
    printf("1 block len: %li pointers\n", pointers_per_block);
    long data_per_iblock = block_size * pointers_per_block;
    printf("1 block:     %li\n", data_per_iblock);
    long data_per_2iblock = data_per_iblock * pointers_per_block;
    printf("2 block:     %li\n", data_per_2iblock);
    long data_per_3iblock = data_per_2iblock * pointers_per_block;
    printf("3 block:     %li\n", data_per_3iblock);

    long acc = 0;
    for (int i = 0; i < 15; i++) {
        switch (i) {
        case 12:
            acc += data_per_iblock;
            break;
        case 13:
            acc += data_per_2iblock;
            break;
        case 14:
            acc += data_per_3iblock;
            break;
        default:
            acc += block_size;
            break;
        }
        printf("block i: up to %li bytes\n", acc);
    }
}

void test_read_indirect(struct ext2_super_block *sb, struct ext2_inode *in,
    off_t offset, size_t len)
{
    size_t read = 0;

    long block_size = 1024 << sb->s_log_block_size;
    long pointers_per_block = block_size / sizeof(uint32_t);
    long data_per_iblock = block_size * pointers_per_block;
    long data_per_2iblock = data_per_iblock * pointers_per_block;
    long data_per_3iblock = data_per_2iblock * pointers_per_block;

    long end_of_11 = block_size * 12;
    long end_of_12 = data_per_iblock + end_of_11;
    long end_of_13 = data_per_2iblock + end_of_12;
    long end_of_14 = data_per_3iblock + end_of_13;

    while (offset < end_of_11) {
        printf("read block %li\n", offset / block_size);
        offset += block_size;
        read += block_size;
        if (read >= len)
            return;
    }

    if (offset < end_of_12)
        printf("[i] read block 12\n");
    while (offset < end_of_12) {
        printf("  read i block %li\n", (offset - end_of_11) / block_size);
        offset += block_size;
        read += block_size;
        if (read >= len)
            return;
    }

    if (offset < end_of_13)
        printf("[i] read block 13\n");
    while (offset < end_of_13) {
        long i_index = (offset - end_of_12) / data_per_iblock;
        printf("  [i] read i block %li\n", i_index);
        long i_begin = end_of_12 + data_per_iblock * i_index;
        long i_end = end_of_12 + data_per_iblock * (i_index + 1);
        while (offset < i_end) {
            printf("    read 2i block %li\n", (offset - i_begin) / block_size);
            offset += block_size;
            read += block_size;
            if (read >= len)
                return;
        }
    }

    if (offset < end_of_14)
        printf("[i] read block 14\n");
    while (offset < end_of_14) {
        long i2_index = (offset - end_of_13) / data_per_2iblock;
        printf("  [i] read i block %li\n", i2_index);
        long i2_begin = end_of_13 + data_per_2iblock * i2_index;
        long i2_end = end_of_13 + data_per_2iblock * (i2_index + 1);
        while (offset < i2_end) {
            long i_index = (offset - i2_begin) / data_per_iblock;
            printf("    [i] read 2i block %li\n", i_index);
            long i_begin = i2_begin + data_per_iblock * i_index;
            long i_end = i2_begin + data_per_iblock * (i_index + 1);
            while (offset < i_end) {
                printf("      read 3i block %li\n",
                    (offset - i_begin) / block_size);
                offset += block_size;
                read += block_size;
                if (read >= len)
                    return;
            }
        }
    }
}

void read_indirect(struct ext2_super_block *sb, struct ext2_inode *in,
    off_t offset, void *buffer, size_t len)
{
    size_t read = 0;
    off_t orig_offset = offset;
#define OFFSET (offset - orig_offset)
#define READ(block_id) \
    do { \
        read_block(block_id, buffer + OFFSET); \
        offset += block_size; \
        read += block_size; \
        if (read >= len) \
            return; \
    } while (0)

    long block_size = 1024 << sb->s_log_block_size;
    long pointers_per_block = block_size / sizeof(uint32_t);
    long data_per_iblock = block_size * pointers_per_block;
    long data_per_2iblock = data_per_iblock * pointers_per_block;
    long data_per_3iblock = data_per_2iblock * pointers_per_block;

    uint32_t i1_buffer[pointers_per_block];
    uint32_t i2_buffer[pointers_per_block];
    uint32_t i3_buffer[pointers_per_block];

    long end_of_11 = block_size * 12;
    long end_of_12 = data_per_iblock + end_of_11;
    long end_of_13 = data_per_2iblock + end_of_12;
    long end_of_14 = data_per_3iblock + end_of_13;

    while (offset < end_of_11) {
        long block_id = offset / block_size;
        READ(in->i_block[block_id]);
    }

    if (offset < end_of_12)
        read_block(in->i_block[12], i1_buffer);
    while (offset < end_of_12) {
        long block_id = (offset - end_of_11) / block_size;
        READ(i1_buffer[block_id]);
    }

    if (offset < end_of_13)
        read_block(in->i_block[13], i1_buffer);
    while (offset < end_of_13) {
        long i_index = (offset - end_of_12) / data_per_iblock;
        read_block(i1_buffer[i_index], i2_buffer);
        long i_begin = end_of_12 + data_per_iblock * i_index;
        long i_end = end_of_12 + data_per_iblock * (i_index + 1);
        while (offset < i_end) {
            long block_id = (offset - i_begin) / block_size;
            READ(i2_buffer[block_id]);
        }
    }

    if (offset < end_of_14)
        read_block(in->i_block[14], i1_buffer);
    while (offset < end_of_14) {
        long i2_index = (offset - end_of_13) / data_per_2iblock;
        read_block(i1_buffer[i2_index], i2_buffer);
        long i2_begin = end_of_13 + data_per_2iblock * i2_index;
        long i2_end = end_of_13 + data_per_2iblock * (i2_index + 1);
        while (offset < i2_end) {
            long i_index = (offset - i2_begin) / data_per_iblock;
            read_block(i_index, i2_buffer);
            long i_begin = i2_begin + data_per_iblock * i_index;
            long i_end = i2_begin + data_per_iblock * (i_index + 1);
            while (offset < i_end) {
                long block_id = (offset - i_begin) / block_size;
                READ(i3_buffer[block_id]);
            }
        }
    }
}

char file_buffer[1024 * 20];

void ext2_info(void)
{
    char buffer[1024];
    read_block(1, buffer);
    struct ext2_super_block sb = *(struct ext2_super_block *)buffer;

    if (sb.s_rev_level == 0)
        sb.s_inode_size = 128;

    ext2_super_block_info(&sb);

    int n_bgs = round_up(sb.s_blocks_count, sb.s_blocks_per_group)
        / sb.s_blocks_per_group;

    printf("block groups: %i\n", n_bgs);
    read_block(2, buffer);
    struct ext2_block_group_descriptor *pbg = (void *)buffer;
    for (int i = 0; i < n_bgs; i++) {
        ext2_block_group_descriptor_info(pbg);
        pbg++;
    }

    struct ext2_inode i2 = get_inode(&sb, 2);
    tree_dir(&sb, &i2);

    numbers(&sb);

    printf("scenario 1:\n");
    test_read_indirect(&sb, NULL, 1024 * 8, 1024 * 8);
    printf("scenario 2:\n");
    test_read_indirect(&sb, NULL, 274432, 1024 * 8);
    printf("scenario 2.5:\n");
    test_read_indirect(&sb, NULL, 274432 - 1024 * 3, 1024 * 8);
    printf("scenario 3:\n");
    test_read_indirect(&sb, NULL, 534528, 1024 * 8);
    printf("scenario 4:\n");
    test_read_indirect(&sb, NULL, 67383296 - 1024 * 3, 1024 * 8);

    struct ext2_inode i2100 = get_inode(&sb, 2100);
    read_indirect(&sb, &i2100, 0, file_buffer, i2100.i_size);
    for (int i = 0; i < i2100.i_size; i += 256)
        printf("%.256s", file_buffer + i);
}
