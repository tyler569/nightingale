#pragma once

#include <nx/list.h>
#include <stddef.h>

class pk {
public:
    pk() = default;
    pk(const pk &) = delete;
    pk(pk &&) = delete;
    pk &operator=(const pk &) = delete;
    pk &operator=(pk &&) = delete;

    char data[2048];
    size_t size;

    nx::list_node socket_node;
};