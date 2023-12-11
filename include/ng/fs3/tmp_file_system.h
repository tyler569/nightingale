#pragma once

#include "file.h"
#include "file_system.h"
#include "inode.h"
#include <nx/vector.h>

auto min(auto a, auto b) { return a < b ? a : b; }

class tmp_inode : public inode {
    void *m_data;
    size_t m_capacity;
    off_t m_size;

public:
    tmp_inode() = default;
    ~tmp_inode() override = default;

    [[nodiscard]] void *data() const { return m_data; }
    [[nodiscard]] off_t size() const { return m_size; }
    void resize_buffer(size_t size)
    {
        if (size > m_capacity) {
            m_data = realloc(m_data, size);
            m_capacity = size;
        }
    }
    void resize_file(size_t size)
    {
        if (m_size > m_capacity) {
            resize_buffer(size);
            m_size = size;
        }
    }
};

class tmp_file : public file {
    tmp_inode *m_inode;
    off_t m_offset;

public:
    tmp_file(file_system *fs, class inode *i, class dentry *d)
        : file(fs, i, d)
    {
    }

    ~tmp_file() override = default;

    ssize_t read(size_t offset, size_t size, char buffer[size]) override
    {
        off_t to_read = min(size, m_inode->size() - offset);
        memcpy(buffer, (char *)m_inode->data() + offset, to_read);
        m_offset += to_read;
        return to_read;
    }

    ssize_t write(size_t offset, size_t size, const char buffer[size]) override
    {
        size_t max_write_addr = offset + size;
        if (max_write_addr > m_inode->size()) {
            m_inode->resize_file(max_write_addr);
        }
        memcpy((char *)m_inode->data() + offset, buffer, size);
        m_offset += size;
        return size;
    }

    [[nodiscard]] size_t size() const override { return m_inode->size(); }

    void truncate(size_t size) override { m_inode->resize_file(size); }
};

class tmp_file_system : public file_system {
    nx::vector<tmp_inode *> m_inodes;

public:
    tmp_file_system() = default;
    ~tmp_file_system() override = default;

    tmp_inode *get_inode(long inode_number) override
    {
        if (inode_number < 0 || inode_number >= m_inodes.size()) {
            return nullptr;
        }
        return m_inodes[inode_number];
    }

    tmp_inode *new_inode() override
    {
        auto *i = new tmp_inode();
        m_inodes.push_back(i);
        return i;
    }

    void destroy_inode(inode *i) override
    {
        nx::print("destroy_inode: not implemented\n");
    }

    void mount(dentry *d) override { }
};
