#pragma once

#include <stddef.h>
#include <sys/types.h>

class nic {
public:
    virtual ~nic() = default;
    virtual void send_packet(const void *data, size_t len) = 0;
    virtual ssize_t recv_packet(void *data, size_t len) = 0;
};