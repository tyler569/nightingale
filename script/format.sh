#!/usr/bin/env bash

CODE="include kernel libc linker user arch"

find $CODE -name '*.[ch]' | xargs clang-format -i
