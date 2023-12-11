#pragma once

#include "dentry.h"
#include "file_system.h"
#include "inode.h"
#include <errno.h>
#include <stddef.h>

class file {
    file_system *m_file_system;
    inode *m_inode;
    dentry *m_dentry;

public:
    file(file_system *fs, inode *i, dentry *d)
        : m_file_system(fs)
        , m_inode(i)
        , m_dentry(d)
    {
    }

    file(const file &) = delete;
    file(file &&) = delete;

    file &operator=(const file &) = delete;
    file &operator=(file &&) = delete;

    virtual ~file() = default;

    file_system *fs() const { return m_file_system; }
    inode *inode() const { return m_inode; }
    dentry *dentry() const { return m_dentry; }

    virtual ssize_t read(size_t offset, size_t size, char buffer[size])
    {
        return -EPERM;
    }
    virtual ssize_t write(size_t offset, size_t size, const char buffer[size])
    {
        return -EPERM;
    }
    [[nodiscard]] virtual size_t size() const { return 0; }
    virtual void truncate(size_t size) { }
    virtual void flush() { }
    virtual void sync() { }
    virtual void close() { }
};