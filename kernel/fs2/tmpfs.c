#define TMPFS_FILESYSTEM_ID 134820

struct tmpfs_inode {
    struct inode vfs_inode;
    void *data;
    size_t length;
    size_t capacity;
};

static struct tmpfs_inode *to_tmpfs_inode(struct inode) {
    assert(inode->filesystem_id == TMPFS_FILESYSTEM_ID);
    struct tmpfs_inode *tmpfs_inode =
        container_of(inode, struct tmpfs_inode, vfs_inode);
    return tmpfs_inode;
}

struct inode *tmpfs_alloc_inode() {
    struct tmpfs_inode *inode = calloc(1, sizeof(struct tmpfs_inode));
    inode->vfs_inode.filesystem_id = TMPFS_FILESYSTEM_ID;
    return &inode->vfs_inode;
}

void tmpfs_dealloc_inode(struct inode *inode) {
    struct tmpfs_inode *tmpfs_inode = to_tmpfs_inode(inode);
    free(tmpfs_inode);
}

int tmpfs_mount(struct file_system *fs, struct dentry *mountpoint) {
    // does this part happen here or in `mount_filesystem`?
    // mountpoint->mounted_filesystem = fs;
    // mountpoint->flags |= DENTRY_IS_MOUNTPOINT;
}

struct file_system_operations tmpfs_ops = {
    .alloc_inode = tmpfs_alloc_inode,
    .dealloc_inode = tmpfs_dealloc_inode,
    .mount = tmpfs_mount,
};

////////////////

int tmpfs_create(struct inode *inode, struct dentry *dentry, int mode) {
    // All of this could be in the generic function
    // struct tmpfs_inode *inode = to_tmpfs_inode(inode);
    // assert(!dentry->inode);

    // struct tmpfs_inode *child = tmpfs_alloc_inode();
    // child->vfs_inode->mode = mode;
    // dentry->inode = child;

    // There's an argument to be made for allocating the ->data buffer here
    // I think.
}

int tmpfs_lookup(struct inode *inode, struct dentry *dentry) {
    // Again, this could be the generic impl.
    // tmpfs always knows about its nodes, so if the dentry->inode is NULL
    // the child does not exist.
    // That said, someone has to actually try filling that in at some point,
    // not entirely sure when that happens.
    //
    // if (!dentry->inode)  return 0;
}

struct inode_operations tmpfs_inode_operations = {
    // .create = tmpfs_create,
    // .lookup = tmpfs_lookup,
    // ...
};

ssize_t tmpfs_read(struct fs2_file *fs2_file, char *buffer, size_t len) {
    struct tmpfs_inode *tmpfs_inode = to_tmpfs_inode(fs2_file->inode);
    if (tmpfs_inode->length >= fs2_file->offset)
        return 0;

    size_t n_to_read = min(len, tmpfs_inode->length - fs2_file->offset);
    memcpy(buffer, tmpfs_inode + fs2_file->offset, n_to_read);
    return n_to_read;
}

// _open cares about O_TRUNC (->length = 0) and O_APPEND (->offset = ->length)

ssize_t tmpfs_write(struct fs2_file *fs2_file, const char *buffer, size_t len) {
    struct tmpfs_inode *tmpfs_inode = to_tmpfs_inode(fs2_file->inode);

    spin_lock(&inode->lock);
    size_t new_length = max(tmpfs_inode->length, fs2_file->offset + len);
    if (new_length > tmpfs_inode->capacity) {
        // reallocate
        void *new_data = malloc(new_length * 3 / 2);
        if (!new_data) {
            spin_unlock(&inode->lock);
            return -ENOMEM;
        }
        memcpy(new_data, tmpfs_inode->data, tmpfs_inode->length);
    }

    memcpy(tmpfs_inode->data + fs2_file->offset, buffer, len);
    spin_unlock(&inode->lock);
    // Whose job is it to synchronize this?
    // Should there be a lock in struct fs2_file or do we tell usermode not to
    // write to the same fs2_file in multiple threads at the same time?
    // The behavior would be nondeterministic anyway as far as what comes
    // out the other end, so maybe having userspace synchronize it makes
    // the most sense.
    fs2_file->offset += len;
    return len;
}
