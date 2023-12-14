#pragma once

#include "file_system.h"
#include "inode.h"
#include <nx/list.h>
#include <string.h>

class dentry {
    nx::list_node m_children_node;
    nx::list<dentry, &dentry::m_children_node> m_children;
    dentry *m_parent;

    file_system *m_file_system;
    inode *m_inode;
    const char m_name[64] {};

public:
    dentry()
        : m_parent(nullptr)
        , m_file_system(nullptr)
        , m_inode(nullptr)
    {
    }

    explicit dentry(const char *name)
        : m_parent(nullptr)
        , m_file_system(nullptr)
        , m_inode(nullptr)
    {
        strncpy((char *)m_name, name, sizeof(m_name));
    }

    dentry(const char *name, dentry *parent)
        : m_parent(parent)
        , m_file_system(parent->m_file_system)
        , m_inode(nullptr)
    {
        strncpy((char *)m_name, name, sizeof(m_name));
        parent->m_children.push_back(*this);
    }

    dentry(const dentry &) = delete;
    dentry(dentry &&) = delete;

    dentry &operator=(const dentry &) = delete;
    dentry &operator=(dentry &&) = delete;

    ~dentry() = default;

    const char *name() const { return m_name; }
    dentry *parent() const { return m_parent; }
    const nx::list<dentry, &dentry::m_children_node> &children() const
    {
        return m_children;
    }

    dentry &find_child(const char *name)
    {
        for (auto &child : m_children) {
            if (strcmp(child.name(), name) == 0) {
                return child;
            }
        }
        return add_child(new dentry(name, this));
    }

    dentry &add_child(dentry *child)
    {
        m_children.push_back(*child);
        return *child;
    }

    bool is_negative() const { return m_inode == nullptr; }
    inode *inode() const { return m_inode; }

    void set_inode(::inode *i) { m_inode = i; }
};