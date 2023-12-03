#!/usr/bin/env bash

CODE="include kernel libc linker user"

find $CODE -name '*.[ch]' | xargs clang-format -i
find $CODE -name '*.cpp' | xargs clang-format -i
