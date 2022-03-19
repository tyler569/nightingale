#pragma once
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include "types.h"

struct file_operations {
    ssize_t (*read)(struct file *, char *buffer, size_t len);
    ssize_t (*write)(struct file *, const char *buffer, size_t len);
    int (*ioctl)(struct file *, int request, void *argp);
    off_t (*seek)(struct file *, off_t offset, int whence);
    ssize_t (*getdents)(struct file *, struct ng_dirent *, size_t);
};

extern struct file_operations default_file_ops;

struct file {
    struct dentry *dentry;
    struct inode *inode;
    const struct file_operations *ops;
    enum open_flags flags;

    off_t offset;
    void *extra;
    size_t len;
    size_t size;
};

// Get a file from the running_process's fd table
struct file *get_file(int fd);
int add_file(struct file *fd);
int add_file_at(struct file *fd, int at);
struct file *remove_file(int fd);

ssize_t default_read(struct file *, char *, size_t);
ssize_t default_write(struct file *, const char *, size_t);

bool read_mode(struct file *file);
bool write_mode(struct file *file);
bool has_permission(struct inode *inode, int flags);
bool read_permission(struct inode *inode);
bool write_permission(struct inode *inode);
bool execute_permission(struct inode *inode);

ssize_t read_file(struct file *file, char *buffer, size_t len);
ssize_t write_file(struct file *file, const char *buffer, size_t len);
int ioctl_file(struct file *file, int request, void *argp);
off_t seek_file(struct file *file, off_t offset, int whence);
ssize_t getdents_file(struct file *file, struct ng_dirent *buf, size_t len);

struct process;
struct file *clone_file(struct file *file);
struct file **clone_all_files(struct process *proc);
void close_all_files(struct process *proc);
