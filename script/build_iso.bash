#!/usr/bin/env bash

set -e
set -o pipefail

mkdir -p build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/CMake/CMakeToolchain.txt -G Ninja ..
ninja install | grep -v Up-to-date

mkdir -p isodir/boot/grub

pushd sysroot > /dev/null
tar --sort=name -c -f ../isodir/boot/initfs.tar *
popd > /dev/null

cp kernel/nightingale_kernel isodir/boot
cp ../kernel/grub.cfg isodir/boot/grub
grub-mkrescue -o ../ngos.iso isodir


