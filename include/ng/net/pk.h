#pragma once

#include <nx/list.h>
#include <stddef.h>

class pk {
public:
    pk() = default;

    pk(pk &&) = delete;
    pk &operator=(pk &&) = delete;

    nx::list_node socket_node;
    size_t size {};

    char data[2048] {};
};