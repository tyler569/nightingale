#!/usr/bin/env bash

CODE="include kernel libc linker user"

find $CODE -name '*.[ch]' | xargs clang-format -i
