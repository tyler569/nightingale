#pragma once

#include "file_system.h"
#include "inode.h"
#include <nx/list.h>
#include <string.h>

namespace fs3 {

class dentry {
public:
    dentry();
    explicit dentry(const char *name);
    dentry(const char *name, dentry *parent);
    ~dentry() = default;

    dentry(dentry &&) = delete;
    dentry &operator=(dentry &&) = delete;

    const char *name() const { return m_name; }
    dentry *parent() const { return m_parent; }

    dentry &find_child(const char *name);
    dentry &add_child(dentry *child);

    bool is_negative() const { return m_inode == nullptr; }
    inode *inode() const { return m_inode; }

    void set_inode(class inode *i) { m_inode = i; }

private:
    nx::list_node m_children_node;
    nx::list<dentry, &dentry::m_children_node> m_children;
    dentry *m_parent;

    file_system *m_file_system;
    class inode *m_inode;
    const char m_name[64] {};

public:
    nx::list<dentry, &dentry::m_children_node> &children()
    {
        return m_children;
    }
};

}
