#!/usr/bin/env bash
# set -x
function build() {
    cd libm
    ARCH=x86_64 CC=x86_64-nightingale-gcc SYSINC=../../sysroot/usr/include SYSLIB=../../sysroot/usr/lib make -j 8 install
    cd -
    cd lua
    CC=x86_64-nightingale-gcc SYSUSR=../../../sysroot/usr CFLAGS="-g -static" LDFLAGS="-g -static" make -j 8
    SYSUSR=../../../sysroot/usr make install
    cd -
}

function clean() {
    cd libm
    make clean
    cd -
    cd lua
    make clean
    cd -
}

if [[ "$1" == "clean" ]]; then
    clean
else
    build
fi
