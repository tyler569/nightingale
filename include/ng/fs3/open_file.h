#pragma once

#include "dentry.h"

class open_file {
public:
    virtual void read(void *buffer, size_t len) = 0;
    virtual void write(const void *buffer, size_t len) = 0;
    virtual void seek(off_t offset, int whence) = 0;
    virtual void close() = 0;
    virtual ~open_file() = default;
};