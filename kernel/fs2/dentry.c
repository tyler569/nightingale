#include <basic.h>
#include <assert.h>
#include <ng/string.h>
#include <ng/thread.h>
#include <stdlib.h>
#include "types.h"
#include "dentry.h"
#include "file_system.h"
#include "file.h"
#include "inode.h"

struct dentry *global_root_dentry;

struct inode *dentry_inode(struct dentry *dentry) {
    if (dentry->flags & DENTRY_IS_MOUNTPOINT) {
        return dentry->file_system->root_inode;
    } else {
        return dentry->inode;
    }
}

struct dentry *new_dentry() {
    struct dentry *dentry = zmalloc(sizeof(struct dentry));
    list_init(&dentry->children);
    return dentry;
}


// Cache child Create/Find/Remove

struct dentry *add_child(
    struct dentry *dentry,
    const char *name,
    struct inode *inode
) {
    if (inode == NULL) {
        struct dentry *new = new_dentry();
        new->name = strdup(name);
        new->parent = dentry;
        new->inode = NULL;
        list_append(&dentry->children, &new->children_node);
        return new;
    }
    struct dentry *child = find_child(dentry, name);
    if (child->inode) {
        // it already existed.
        return NULL;
    }
    child->inode = inode;
    inode->dentry_refcnt += 1;
    return child;
}


struct dentry *find_child(struct dentry *dentry, const char *name) {
    if (!dentry->inode) {
        return NULL;
    }

    if (dentry->inode->type != FT_DIRECTORY) {
        return NULL;
    }

    list_for_each(struct dentry, d, &dentry->children, children_node) {
        if (strcmp(d->name, name) == 0) {
            return d;
        }
    }

    return add_child(dentry, name, NULL);
}

int delete_entry(struct dentry *dentry) {
    // if !list_empty(children)  fail
    // close fs2_file?
    // assert fs2_file already closed?
    // Presumably this is only called after inode->delete for in-menory
    // filesystems, but it could be called to evict entries from the
    // cache for filesystems with persistence.
    return 0;
}

// Path resolution

struct dentry *resolve_path_from(struct dentry *cursor, const char *path) {
    char buffer[128] = {0};

    if (path[0] == '/') {
        cursor = running_process->root;
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
    } while (path[0] && cursor->inode);

    if (path[0]) {
        return NULL;
    }

    return cursor;
}

struct dentry *resolve_path(const char *path) {
    return resolve_path_from(running_process->root, path);
}

struct dentry *resolve_atfd(int fd) {
    struct dentry *root = running_process->root;

    if (fd == AT_FDCWD) {
        root = running_thread->cwd2;
    } else if (fd >= 0) {
        struct fs2_file *fs2_file = get_file(fd);
        if (!fs2_file)
            return TO_ERROR(-EBADF);
        root = fs2_file->dentry;
    }

    return root;
}

// Reverse path resolution

static char *pathname_rec(
    struct dentry *dentry,
    char *buffer,
    size_t len,
    bool root
) {
    if (dentry->parent == dentry) {
        buffer[0] = '/';
        buffer[1] = 0;
        return buffer + 1;
    }

    char *after = pathname_rec(dentry->parent, buffer, len, false);

    size_t rest = len - (after - buffer);
    char *next = memccpy(after, dentry->name, 0, rest);
    if (!root) {
        next[-1] = '/';
        next[0] = 0;
    }
    return next;
}

int pathname(struct fs2_file *fs2_file, char *buffer, size_t len) {
    struct dentry *dentry = fs2_file->dentry;
    char *after = pathname_rec(dentry, buffer, len, true);
    return after - buffer;;
}
