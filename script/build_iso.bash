#!/usr/bin/env bash

if command -v grub2-mkrescue; then
  GRUB2_MKRESCUE=grub2-mkrescue
elif command -v grub-mkrescue; then
  GRUB2_MKRESCUE=grub-mkrescue
else
  echo "grub2-mkrescue not found on your system"
  exit 1
fi

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
$GRUB2_MKRESCUE -o ../ngos.iso isodir


