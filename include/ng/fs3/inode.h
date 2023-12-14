#pragma once

#include "dentry.h"
#include "open_file.h"
#include <sys/types.h>

namespace fs3 {

class inode {
public:
    virtual ~inode() = default;

    virtual open_file *open(int flags, int mode) = 0;
    virtual ssize_t read(size_t offset, void *buffer, size_t len) { return 0; }
    virtual ssize_t write(size_t offset, const void *buffer, size_t len)
    {
        return 0;
    }
};

}