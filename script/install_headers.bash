#!/usr/bin/env bash

mkdir -p sysroot/bin
mkdir -p sysroot/usr/include
mkdir -p sysroot/usr/man/man1
mkdir -p sysroot/usr/bin
mkdir -p sysroot/usr/lib
mkdir -p sysroot/usr/share

function copy() {
    # cp --dereference --recursive --no-target-directory --preserve=timestamps "$1" "$2"
    rsync -a "$1" "$2"
}

copy include sysroot/usr
copy libc/include sysroot/usr
copy libm/include sysroot/usr
copy kernel/include sysroot/usr
copy x86/include sysroot/usr
copy fs/include sysroot/usr
copy linker/include sysroot/usr

find user -maxdepth 1 -type f -not -name '*.c' -exec cp {} sysroot/bin \;

# update the timestamp for make
touch sysroot
