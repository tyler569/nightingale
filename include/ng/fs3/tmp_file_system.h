#pragma once

#include "file_system.h"

class tmp_file_system : public file_system {
public:
    tmp_file_system() = default;
    ~tmp_file_system() = default;

    inode *get_inode(long inode_number) override;
    inode *new_inode() override;
    void destroy_inode(inode *i) override;

    void mount(dentry *d) override;
};
