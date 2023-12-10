#include <assert.h>
#include <errno.h>
#include <ng/common.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/types.h>
#include <ng/string.h>
#include <ng/thread.h>
#include <stdbool.h>
#include <stdlib.h>

dentry *global_root_dentry;

extern inline inode *dentry_inode(dentry *dentry);
extern inline file_system *dentry_file_system(dentry *dentry);

dentry *new_dentry()
{
    auto *d = (dentry *)zmalloc(sizeof(dentry));
    list_init(&d->children);
    return d;
}

void maybe_delete_dentry(dentry *d)
{
    if (d->file_refcnt)
        return;
    if (!list_empty(&d->children))
        return;
    if (d->parent)
        return;

    if (d->inode)
        detach_inode(d);
    if (d->name)
        free((void *)d->name);
    free(d);
}

// Cache child Create/Find/Remove

dentry *add_child(dentry *d, const char *name, inode *i)
{
    if (i == nullptr) {
        auto *new_d = new_dentry();
        new_d->name = strdup(name);
        new_d->parent = d;
        new_d->inode = nullptr;
        if (d->mounted_file_system)
            new_d->file_system = d->mounted_file_system;
        else
            new_d->file_system = d->file_system;
        list_append(&d->children, &new_d->children_node);
        return new_d;
    }
    dentry *child = find_child(d, name);
    if (child->inode) {
        // it already existed.
        return static_cast<dentry *>(TO_ERROR(-EEXIST));
    }
    attach_inode(child, i);
    return child;
}

dentry *find_child(dentry *d, const char *name)
{
    dentry *found = nullptr;
    inode *inode = dentry_inode(d);
    if (!inode)
        return static_cast<dentry *>(TO_ERROR(-ENOENT));
    if (inode->type != FT_DIRECTORY)
        return static_cast<dentry *>(TO_ERROR(-ENOTDIR));

    if (inode->ops->lookup)
        return inode->ops->lookup(d, name);

    list_for_each (dentry, child, &d->children, children_node) {
        if (strcmp(child->name, name) == 0) {
            found = child;
            break;
        }
    }

    if (found) {
        if (found->mounted_file_system)
            found = found->mounted_file_system->root;
        return found;
    }

    return add_child(d, name, nullptr);
}

dentry *unlink_dentry(dentry *dentry)
{
    list_remove(&dentry->children_node);
    dentry->parent = nullptr;
    maybe_delete_dentry(dentry);
    return dentry;
}

void destroy_dentry_tree(dentry *dentry) { }

int attach_inode(dentry *dentry, inode *inode)
{
    if (dentry_inode(dentry))
        return -EEXIST;
    dentry->inode = inode;
    atomic_fetch_add(&inode->dentry_refcnt, 1);
    return 0;
}

void detach_inode(dentry *dentry)
{
    assert(dentry->inode);
    atomic_fetch_sub(&dentry->inode->dentry_refcnt, 1);
    maybe_delete_inode(dentry->inode);
    dentry->inode = nullptr;
}

// Path resolution

dentry *resolve_path_from_loopck(
    dentry *cursor, const char *path, bool follow, int n_symlinks)
{
    inode *inode;
    char buffer[128] = { 0 };

    if (n_symlinks > 8)
        return static_cast<dentry *>(TO_ERROR(-ELOOP));

    if (path[0] == '/') {
        cursor = get_running_root();
        path++;
        if (path[0] == 0) {
            return cursor;
        }
    }

    if (!cursor)
        cursor = global_root_dentry;

    assert(cursor);

    do {
        path = strccpy(buffer, path, '/');

        if (strcmp(buffer, "..") == 0) {
            cursor = cursor->parent;
        } else if (strcmp(buffer, ".") == 0) {
            continue;
        } else if (buffer[0] == 0) {
            continue;
        } else {
            cursor = find_child(cursor, buffer);
        }

        while (follow && !IS_ERROR(cursor) && (inode = dentry_inode(cursor))
            && inode->type == FT_SYMLINK) {
            char link_buffer[64] = { 0 };
            const char *dest;
            if (inode->ops->readlink) {
                ssize_t err = inode->ops->readlink(inode, link_buffer, 64);
                if (err < 0)
                    return static_cast<dentry *>(TO_ERROR(err));
                dest = link_buffer;
            } else {
                dest = inode->symlink_destination;
            }

            cursor = resolve_path_from_loopck(
                cursor->parent, dest, true, n_symlinks + 1);

            if (IS_ERROR(cursor))
                return cursor;
        }
    } while (path[0] && (cursor->inode || cursor->mounted_file_system));

    if (path[0] || !cursor)
        return static_cast<dentry *>(TO_ERROR(-ENOENT));

    return cursor;
}

dentry *resolve_path_from(dentry *cursor, const char *path, bool follow)
{
    return resolve_path_from_loopck(cursor, path, follow, 0);
}

dentry *resolve_path(const char *path)
{
    return resolve_path_from(get_running_root(), path, true);
}

dentry *resolve_atfd(int fd)
{
    dentry *root = get_running_root();

    if (fd == AT_FDCWD) {
        root = get_running_cwd();
    } else if (fd >= 0) {
        file *file = get_file(fd);
        if (!file)
            return static_cast<dentry *>(TO_ERROR(-EBADF));
        root = file->dentry;
    }

    return root;
}

dentry *resolve_atpath(int fd, const char *path, bool follow)
{
    dentry *at = resolve_atfd(fd);
    if (IS_ERROR(at) || !path)
        return at;
    // if (path[0] == 0 && !(fd & AT_EMPTY_PATH)) // something
    dentry *dentry = resolve_path_from(at, path, follow);
    return dentry;
}

// Reverse path resolution

static char *pathname_rec(dentry *dentry, char *buffer, size_t len, bool root)
{
    if (dentry->parent == dentry) {
        buffer[0] = '/';
        buffer[1] = 0;
        return buffer + 1;
    }

    char *after = pathname_rec(dentry->parent, buffer, len, false);

    size_t rest = len - (after - buffer);
    char *next = (char *)memccpy(after, dentry->name, 0, rest);
    if (!root) {
        next[-1] = '/';
        next[0] = 0;
    }
    return next;
}

int pathname(dentry *dentry, char *buffer, size_t len)
{
    char *after = pathname_rec(dentry, buffer, len, true);
    return after - buffer;
}
