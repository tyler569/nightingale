#include <ng/fs3/dentry.h>

namespace fs3 {

dentry::dentry()
    : m_parent(nullptr)
    , m_file_system(nullptr)
    , m_inode(nullptr)
{
}

dentry::dentry(const char *name)
    : m_parent(nullptr)
    , m_file_system(nullptr)
    , m_inode(nullptr)
{
    strncpy((char *)m_name, name, sizeof(m_name));
}

dentry::dentry(const char *name, dentry *parent)
    : m_parent(parent)
    , m_file_system(parent->m_file_system)
    , m_inode(nullptr)
{
    strncpy((char *)m_name, name, sizeof(m_name));
    parent->m_children.push_back(*this);
}

dentry &dentry::find_child(const char *name)
{
    for (auto &child : m_children) {
        if (strcmp(child.name(), name) == 0) {
            return child;
        }
    }
    return add_child(new dentry(name, this));
}

dentry &dentry::add_child(dentry *child)
{
    m_children.push_back(*child);
    return *child;
}

}