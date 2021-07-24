#!/usr/bin/env bash

mkdir -p build
cd build

cmake -G Ninja ..
ninja install | grep -v Up-to-date

mkdir -p isodir/boot/grub

pushd sysroot > /dev/null
tar cf ../isodir/boot/initfs.tar *
popd > /dev/null

cp kernel/nightingale_kernel isodir/boot
cp ../kernel/grub.cfg isodir/boot/grub
grub-mkrescue -o ../ngos.iso isodir


