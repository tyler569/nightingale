#pragma once

#include "dentry.h"
#include <sys/types.h>

namespace fs3 {

class open_file {
public:
    virtual ssize_t read(void *buffer, size_t len) = 0;
    virtual ssize_t write(const void *buffer, size_t len) = 0;
    virtual int seek(off_t offset, int whence) = 0;
    virtual int close() = 0;
    virtual ~open_file() = default;
};

}