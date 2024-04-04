#pragma once

#include "dentry.h"
#include "inode.h"
#include "open_file.h"
#include <errno.h>
#include <ng/net/pk.h>
#include <ng/syscall.h>
#include <sys/types.h>

namespace fs3 {

class socket_inode;

class socket_open_file : public open_file {
    socket_inode *m_inode;

public:
    socket_open_file(socket_inode *inode)
        : m_inode(inode)
    {
    }

    ssize_t read(void *buffer, size_t len) override;
    ssize_t write(const void *buffer, size_t len) override;
    int seek(off_t offset, int whence) override;
    int close() override;
};

class socket_inode : public inode {
public:
    socket_inode() = default;
    ~socket_inode() override = default;

    socket_inode(socket_inode &&) = delete;
    socket_inode &operator=(socket_inode &&) = delete;

    socket_open_file *open(int flags, int mode) override;

    pk *pop_pk();
    void push_pk(pk &pk);

private:
    nx::list<pk, &pk::socket_node> m_pks;
};

}

extern "C" sysret sys_fs3_socket();
