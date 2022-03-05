#!/usr/bin/env bash

set -eo pipefail

export CLICOLOR_FORCE=1

if command -v grub2-mkrescue; then
  GRUB2_MKRESCUE=grub2-mkrescue
elif command -v grub-mkrescue; then
  GRUB2_MKRESCUE=grub-mkrescue
else
  echo "grub2-mkrescue not found on your system"
  exit 1
fi

if [[ "${USE_GCC:-0}" != "0" ]]; then
    BUILD_DIR=build-gcc
    mkdir -p $BUILD_DIR
    pushd $BUILD_DIR > /dev/null
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/CMake/CMakeToolchain-gcc.txt -G Ninja ..
else
    BUILD_DIR=build-clang
    mkdir -p $BUILD_DIR
    pushd $BUILD_DIR > /dev/null
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/CMake/CMakeToolchain.txt -G Ninja ..
fi
ninja install | grep -v Up-to-date
ln -sfT "$BUILD_DIR" ../build

mkdir -p isodir/boot/grub

pushd sysroot > /dev/null
tar -c -f ../isodir/boot/initfs.tar *
popd > /dev/null

cp kernel/nightingale_kernel isodir/boot
cp ../kernel/grub.cfg isodir/boot/grub
$GRUB2_MKRESCUE -o ../ngos.iso isodir 2>grub-mkrescue.log
