#!/bin/bash

BINUTILS_VERSION="2.33.1"
GCC_VERSION="9.2.0"
PARALLEL=4

PREFIX="$HOME"
PATH="$PREFIX/bin:$PATH"
TARGET=x86_64-elf

# if there's old builds lying around
rm -rf build-binutils.sh build-binutils binutils-${BINUTILS_VERSION}
rm -rf build-gcc.sh build-gcc gcc-${GCC_VERSION}

mkdir -p "$PREFIX/src"
cd "$PREFIX/src"

wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz
wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz

tar zxf binutils-${BINUTILS_VERSION}.tar.gz
tar zxf gcc-${GCC_VERSION}.tar.gz

rm binutils-${BINUTILS_VERSION}.tar.gz gcc-${GCC_VERSION}.tar.gz

### binutils

cd $PREFIX/src
mkdir build-binutils
cd build-binutils
../binutils-${BINUTILS_VERSION}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror --with-sysroot
#../binutils-${BINUTILS_VERSION}/configure --prefix="$PREFIX" --disable-nls --disable-werror
make -j $PARALLEL
make install

cd $PREFIX/src
#rm -rf build-binutils.sh build-binutils binutils-${BINUTILS_VERSION}

### GCC

cd $PREFIX/src
mkdir build-gcc
cd build-gcc
../gcc-${GCC_VERSION}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++
#../gcc-${GCC_VERSION}/configure --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --disable-multilib
make -j $PARALLEL all-gcc
make -j $PARALLEL all-target-libgcc
make install-gcc
make install-target-libgcc

cd $PREFIX/src
#rm -rf build-gcc.sh build-gcc gcc-${GCC_VERSION}

### libstdc++

# cd $PREFIX/src
# mkdir build-libstdc++
# cd build-libstdc++
# ../gcc-${GCC_VERSION}/libstdc++-v3/configure --prefix="$PREFIX" --disable-nls --disable-multilib --with-gxx-include-dir=$PREFIX/lib/x86_64-pc-linux-gnu/include/c++/${GCC_VERSION}
# make -j $PARALLEL
# make install

