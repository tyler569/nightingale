#!/usr/bin/env bash

find fs include kernel libc linker modules sh user x86 \
    -name '*.[ch]' | xargs clang-format -i
