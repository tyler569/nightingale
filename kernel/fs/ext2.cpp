#include <assert.h>
#include <ext2.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-function"

/*
 * Naming:
 *
 * ext2_* : on-disk / standard structures
 * e2_*   : in-kernel VFS structures
 */

struct e2_file_system {
    file_system file_system;
    ext2_super_block super_block;
};

struct e2_inode {
    inode inode;
    ext2_inode ext2_inode;
};

extern file_system_operations e2_file_system_operations;
extern inode_operations e2_inode_operations;
extern file_operations e2_file_operations;

file_system *new_e2_file_system();

// ext2.c

// file_system_operations
inode *e2_new_inode(file_system *);
inode *e2_get_inode(file_system *, long);
void e2_destroy_inode(inode *);

// inode_operations
int e2_open(inode *, file *);
int e2_close(inode *, file *);
ssize_t e2_readlink(inode *, char *, size_t);
dentry *e2_lookup(dentry *, const char *);

// file_operations
ssize_t e2_read(file *, char *buffer, size_t len);
ssize_t e2_write(file *, const char *buffer, size_t len);
int e2_ioctl(file *, int request, void *argp);
off_t e2_seek(file *, off_t offset, int whence);
ssize_t e2_getdents(file *, dirent *, size_t);

extern "C" {
int read_sector(long, void *);
}

file_system_operations e2_file_system_operations = {
    .new_inode = e2_new_inode,
    .get_inode = e2_get_inode,
    .destroy_inode = e2_destroy_inode,
};

inode_operations e2_inode_operations = {
    .open = e2_open,
    .close = e2_close,
    .readlink = e2_readlink,
    .lookup = e2_lookup,
};

file_operations e2_file_operations = {
    .read = e2_read,
    .write = e2_write,
    .ioctl = e2_ioctl,
    .seek = e2_seek,
    .getdents = e2_getdents,
};

file_system *new_e2_file_system()
{
    auto *fs = (e2_file_system *)zmalloc(sizeof(e2_file_system));
    file_system *file_system = &fs->file_system;
    file_system->ops = &e2_file_system_operations;
    list_init(&file_system->inodes);

    inode *root_inode = file_system->ops->get_inode(file_system, 2);
    dentry *root = new_dentry();
    root->parent = root;
    root->file_system = file_system;
    attach_inode(root, root_inode);
    file_system->root = root;

    return file_system;
}

// ext2 utility functions

static int num_bgs(ext2_super_block *sb)
{
    return ROUND_UP(sb->s_blocks_count, sb->s_blocks_per_group)
        / sb->s_blocks_per_group;
}

// ext2 disk functions

int read_sector(long, void *);
int write_sector(long, void *);

static void read_block(long block_num, void *buffer)
{
    read_sector(block_num * 2, buffer);
    read_sector(block_num * 2 + 1, PTR_ADD(buffer, 512));
}

static void write_block(long block_num, void *buffer)
{
    write_sector(block_num * 2, buffer);
    write_sector(block_num * 2 + 1, PTR_ADD(buffer, 512));
}

static ext2_super_block get_sb()
{
    char buffer[1024];
    read_block(1, buffer);
    ext2_super_block *sb = (ext2_super_block *)buffer;
    return *sb;
}

static void put_sb(ext2_super_block *sb)
{
    char buffer[1024];
    *(ext2_super_block *)buffer = *sb;
    write_block(1, buffer);
}

static ext2_block_group_descriptor get_bg(ext2_super_block *sb, long bg_number)
{
    long block_index = bg_number / BG_PER_BLOCK;
    long block_offset = bg_number % BG_PER_BLOCK;
    ext2_block_group_descriptor bgs[BG_PER_BLOCK];
    read_block(2 + block_index, bgs);

    return bgs[block_offset];
}

static void put_bg(
    ext2_super_block *sb, long bg_number, ext2_block_group_descriptor *bg)
{
    long block_index = bg_number / BG_PER_BLOCK;
    long block_offset = bg_number % BG_PER_BLOCK;
    ext2_block_group_descriptor bgs[BG_PER_BLOCK];
    read_block(2 + block_index, bgs);
    bgs[block_offset] = *bg;
    write_block(2 + block_index, bgs);
}

static bool inode_debug = false;

static ext2_inode get_inode(ext2_super_block *sb, long inode_number)
{
    assert(inode_number > 0);

    if (inode_debug)
        printf("ext2_inode: %li ", inode_number);
    inode_number -= 1;

    long bg_number = inode_number / sb->s_inodes_per_group;
    ext2_block_group_descriptor bg = get_bg(sb, bg_number);
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

    auto *i = (ext2_inode *)PTR_ADD(buffer, block_offset * sb->s_inode_size);
    return *i;
}

static void put_inode(ext2_super_block *sb, long inode_number, ext2_inode *i)
{
    assert(inode_number > 0);

    if (inode_debug)
        printf("ext2_inode: %li ", inode_number);
    inode_number -= 1;

    long bg_number = inode_number / sb->s_inodes_per_group;
    ext2_block_group_descriptor bg = get_bg(sb, bg_number);
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
    auto *pi = (ext2_inode *)PTR_ADD(buffer, block_offset * sb->s_inode_size);
    *pi = *i;
    write_block(bg.bg_inode_table + block_index, buffer);
}

static inode *alloc_inode(file_system *file_system)
{
    auto *fs = container_of(e2_file_system, file_system, file_system);
    ext2_super_block *sb = &fs->super_block;

    // find a block_group with available inodes.
    // find the first available inode.
    // create a inode with the proper inode number
    // mark the inode allocated in the bitmap
    int i;
    int max = num_bgs(sb);
    ext2_block_group_descriptor bg {};

    for (i = 0; i < max; i++) {
        bg = get_bg(sb, i);
        if (bg.bg_free_inodes_count > 0)
            break;
    }

    if (i == max) {
        printf("no inodes available\n");
        return nullptr;
    }

    char buffer[1024];
    long inode_number = -1;
    read_block(bg.bg_inode_bitmap, buffer);
    for (int i = 0; i < 1024; i++) {
        for (int b = 0; b < 8; b++) {
            if (!(buffer[i] & (1 << b))) {
                buffer[i] |= (1 << b);
                write_block(bg.bg_inode_bitmap, buffer);
                inode_number = i * 8 + b + 1;
                break;
            }
        }
    }
    assert(inode_number != -1);

    bg.bg_free_inodes_count -= 1;
    put_bg(sb, i, &bg);

    inode *inode = file_system->ops->new_inode(file_system);
    inode->inode_number = inode_number;
    return inode;
}

static void read_indirect(ext2_super_block *sb, ext2_inode *in, off_t offset,
    void *buffer, size_t len)
{
    size_t read = 0;
    off_t orig_offset = offset;
#define OFFSET (offset - orig_offset)
#define READ(block_id) \
    do { \
        read_block(block_id, PTR_ADD(buffer, OFFSET)); \
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
#undef OFFSET
#undef READ
}

// file_system_operations
inode *e2_new_inode(file_system *fs)
{
    e2_inode *i = (e2_inode *)zmalloc(sizeof(e2_inode));
    i->inode.file_system = fs;
    i->inode.inode_number = -1;
    i->inode.ops = &e2_inode_operations;
    i->inode.file_ops = &e2_file_operations;
    return &i->inode;
}

inode *e2_get_inode(file_system *fs, long inode_number)
{
    auto *e2fs = container_of(e2_file_system, file_system, fs);
    inode *inode = e2_new_inode(fs);
    auto *i = container_of(e2_inode, inode, inode);
    ext2_super_block *sb = &e2fs->super_block;
    i->ext2_inode = get_inode(sb, inode_number);
    i->inode.inode_number = inode_number;
    i->inode.len = i->ext2_inode.i_size;
    i->inode.mode = i->ext2_inode.i_mode;
    return &i->inode;
}

void e2_destroy_inode(inode *inode)
{
    auto *i = container_of(e2_inode, inode, inode);
    free(i);
}

// inode_operations
int e2_open(inode *i, file *f) { return -ETODO; }
int e2_close(inode *i, file *f) { return -ETODO; }
ssize_t e2_readlink(inode *i, char *buffer, size_t len) { return -ETODO; }
dentry *e2_lookup(dentry *d, const char *name)
{
    return (dentry *)TO_ERROR(-ETODO);
}

// file_operations
ssize_t e2_read(file *f, char *buffer, size_t len) { return -ETODO; }
ssize_t e2_write(file *f, const char *buffer, size_t len) { return -ETODO; }
int e2_ioctl(file *f, int request, void *argp) { return -ETODO; }
off_t e2_seek(file *f, off_t offset, int whence) { return -ETODO; }
ssize_t e2_getdents(file *f, dirent *d, size_t len) { return -ETODO; }
