#pragma once

class dentry;
class inode;

class file_system {
public:
    virtual ~file_system() = default;

    virtual inode *get_inode(long inode_number) = 0;
    virtual inode *new_inode() = 0;
    virtual void destroy_inode(inode *i) = 0;

    virtual void mount(dentry *d) = 0;
};