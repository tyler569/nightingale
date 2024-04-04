#pragma once

#include <nx/list.h>
#include <stddef.h>

class pk {
public:
    pk() = default;

    pk(const pk &) = delete;
    pk &operator=(const pk &) = delete;

    pk(pk &&) = delete;
    pk &operator=(pk &&) = delete;

    nx::list_node socket_node;
    size_t size {};

    char data[2048] {};
};
