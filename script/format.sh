#!/usr/bin/env bash

CODE="include kernel libc linker user arch"

find $CODE -name '*.[ch]' | xargs clang-format -i
find $CODE -name '*.cpp' | xargs clang-format -i
