#include <assert.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/inode.h>
#include <ng/fs/types.h>
#include <ng/mt/process.h>
#include <ng/mt/thread.h>
#include <ng/thread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

extern "C" {

// Get a file from the running_process's fd table
struct file *get_file(int fd);
// Get a file from someone else's fd table (?)
struct file *get_file_from_process(int fd, struct process *process);

ssize_t default_read(struct file *file, char *buffer, size_t len)
{
    if (file->offset > file->inode->len)
        return 0;
    size_t to_read = MIN(len, file->inode->len - file->offset);
    memcpy(buffer, PTR_ADD(file->inode->data, file->offset), to_read);
    file->offset += to_read;
    return to_read;
}

ssize_t default_write(struct file *file, const char *buffer, size_t len)
{
    if (!file->inode->data) {
        file->inode->data = malloc(1024);
        file->inode->capacity = 1024;
    }

    size_t final_len = file->offset + len;
    if (file->offset + len > file->inode->capacity) {
        size_t resized_len = final_len * 3 / 2;
        file->inode->data = realloc(file->inode->data, resized_len);
        file->inode->capacity = resized_len;
    }

    memcpy(PTR_ADD(file->inode->data, file->offset), buffer, len);
    file->offset += len;
    file->inode->len = MAX(file->inode->len, final_len);
    return len;
}

off_t default_seek(struct file *file, off_t offset, int whence)
{
    off_t new_offset = file->offset;

    switch (whence) {
    case SEEK_SET:
        new_offset = offset;
        break;
    case SEEK_CUR:
        new_offset += offset;
        break;
    case SEEK_END:
        new_offset = file->inode->len + offset;
        break;
    default:
        return -EINVAL;
    }

    if (new_offset < 0)
        return -EINVAL;

    file->offset = new_offset;
    return new_offset;
}

struct file_operations default_file_ops = {};

bool read_mode(struct file *file) { return file->flags & O_RDONLY; }

bool write_mode(struct file *file) { return file->flags & O_WRONLY; }

bool has_permission(struct inode *inode, int flags)
{
    // bootleg implies, truth table:
    // mode  flags allowed
    // 0     0     1
    // 0     1     0
    // 1     0     1
    // 1     1     1

    return (inode->mode & USR_READ || !(flags & O_RDONLY))
        && (inode->mode & USR_WRITE || !(flags & O_WRONLY));
}

bool write_permission(struct inode *i) { return !!(i->mode & USR_WRITE); }
bool read_permission(struct inode *i) { return !!(i->mode & USR_READ); }
bool execute_permission(struct inode *i) { return !!(i->mode & USR_EXEC); }

struct file *get_file(int fd)
{
    if (fd > running_process->files.size()) {
        return nullptr;
    }
    if (running_process->files[fd])
        assert(running_process->files[fd]->magic == FILE_MAGIC);

    return running_process->files[fd];
}

int add_file(struct file *file)
{
    file->magic = FILE_MAGIC;
    for (auto &f : running_process->files) {
        if (!f) {
            f = file;
            return static_cast<int>(&f - running_process->files.data());
        }
    }
    running_process->files.push_back(file);
    return running_process->files.size() - 1;
}

int add_file_at(struct file *file, int at)
{
    file->magic = FILE_MAGIC;
    if (at < 0)
        return -EBADF;
    while (running_process->files.size() < at) {
        running_process->files.push_back(nullptr);
    }

    running_process->files[at] = file;
    return at;
}

struct file *p_remove_file(struct process *proc, int fd)
{
    struct file *file = proc->files[fd];
    if (file)
        assert(file->magic == FILE_MAGIC);

    proc->files[fd] = 0;
    return file;
}

struct file *remove_file(int fd) { return p_remove_file(running_process, fd); }

void close_all_files(struct process *proc)
{
    for (auto &file : proc->files) {
        if (file) {
            close_file(file);
            file = nullptr;
        }
    }
    proc->files.clear();
}

void close_all_cloexec_files(struct process *proc)
{
    for (auto &file : proc->files) {
        if (file && (file->flags & O_CLOEXEC)) {
            close_file(file);
            file = nullptr;
        }
    }
}

struct file *clone_file(struct file *f)
{
    assert(f->magic == FILE_MAGIC);
    auto *new_file = new file();
    *new_file = *f;
    open_file_clone(new_file);
    return new_file;
}

ssize_t read_file(struct file *file, char *buffer, size_t len)
{
    assert(file->magic == FILE_MAGIC);
    if (!read_mode(file))
        return -EPERM;
    if (file->inode->type == FT_DIRECTORY)
        return -EISDIR;

    access_inode(file->inode);

    if (file->ops->read)
        return file->ops->read(file, buffer, len);
    else
        return default_read(file, buffer, len);
}

ssize_t write_file(struct file *file, const char *buffer, size_t len)
{
    assert(file->magic == FILE_MAGIC);
    if (!write_mode(file))
        return -EPERM;
    if (file->inode->type == FT_DIRECTORY)
        return -EISDIR;

    modify_inode(file->inode);

    if (file->ops->write)
        return file->ops->write(file, buffer, len);
    else
        return default_write(file, buffer, len);
}

int ioctl_file(struct file *file, int request, void *argp)
{
    assert(file->magic == FILE_MAGIC);
    modify_inode(file->inode);

    if (file->ops->ioctl)
        return file->ops->ioctl(file, request, argp);
    else {
        if (request == TTY_ISTTY)
            return 0;
        return -ENOTTY;
    }
}

off_t seek_file(struct file *file, off_t offset, int whence)
{
    assert(file->magic == FILE_MAGIC);
    if (file->ops->seek)
        return file->ops->seek(file, offset, whence);
    else
        return default_seek(file, offset, whence);
}

ssize_t getdents_file(struct file *file, struct dirent *buf, size_t len)
{
    assert(file->magic == FILE_MAGIC);
    access_inode(file->inode);

    if (file->ops->getdents) {
        return file->ops->getdents(file, buf, len);
    } else {
        size_t offset = 0;
        size_t index = 0;
        list_for_each (dentry, d, &file->dentry->children, children_node) {
            if (index < file->offset) {
                index += 1;
                continue;
            }
            dirent *dent = (dirent *)PTR_ADD(buf, offset);
            if (!d->inode) {
                continue;
            }
            size_t max_copy = MIN(256, len - sizeof(struct dirent) - offset);
            size_t str_len = strlen(d->name);
            size_t will_copy = MIN(str_len, max_copy);
            if (will_copy < str_len)
                break;
            strncpy(dent->d_name, d->name, max_copy);
            dent->d_type = d->inode->type;
            dent->d_mode = (unsigned short)d->inode->mode;

            size_t reclen
                = sizeof(struct dirent) - 256 + ROUND_UP(will_copy + 1, 8);
            dent->d_reclen = reclen;
            dent->d_ino = d->inode->inode_number;
            dent->d_off = d->inode->len;

            offset += reclen;
            index += 1;
        }
        file->offset = index;
        return offset;
    }
}

ssize_t readlink_inode(struct inode *inode, char *buffer, size_t len)
{
    if (inode->ops->readlink)
        return inode->ops->readlink(inode, buffer, len);
    else if (inode->symlink_destination) {
        size_t str_len = strlen(inode->symlink_destination);
        strncpy(buffer, inode->symlink_destination, len);
        if (str_len > len)
            return -ENAMETOOLONG;
        return str_len;
    } else {
        return -EINVAL;
    }
}

} // extern "C"

nx::vector<file *> clone_all_files(struct process *proc)
{
    nx::vector<file *> new_files;
    for (auto &file : proc->files) {
        if (file) {
            new_files.push_back(clone_file(file));
        } else {
            new_files.push_back(nullptr);
        }
    }
    return new_files;
}
