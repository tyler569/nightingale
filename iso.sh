#!/bin/sh
set -e
. ./build.sh
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
 
cp sysroot/boot/nightingale.kernel isodir/boot/nightingale.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "nightingale" {
	multiboot /boot/nightingale.kernel
}
EOF
grub-mkrescue -o nightingale.iso isodir
