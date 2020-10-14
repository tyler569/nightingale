#!/bin/bash

# Prerequisite: to build gcc you will need GMP, MPFR, and MPC, on ubuntu
# the necessary packages are:
# - libgmp-dev
# - libmpfr-dev
# - libmpc-dev
#
# on fedora:
# sudo dnf install gcc gcc-c++ gmp-devel mpfr-devel libmpc-devel make bison flex wget patch

# set -x

BINUTILS_VERSION="2.33.1"
GCC_VERSION="9.2.0"
PARALLEL=-j20

PREFIX="$HOME/.local"
PATH="$PREFIX/bin:$PATH"
TARGET=x86_64-nightingale
# TARGET=i686-nightingale

# run this script in nightingale/toolchain
BUILDDIR=$(pwd)
NGDIR=$(realpath ..)
SYSROOT=$(realpath ../sysroot)

echo "building binutils ${BINUTILS_VERSION} and gcc ${GCC_VERSION}"
echo "with nightingale patches"

# install headers to sysroot
echo "installing headers to sysroot"
cd ..
sh sysroot.sh
cd -

echo "cleaning up old builds"
rm -rf build-binutils.sh build-binutils binutils-${BINUTILS_VERSION}
rm -rf build-gcc.sh build-gcc gcc-${GCC_VERSION}

BINUTILS_TAR="binutils-${BINUTILS_VERSION}.tar.gz"
GCC_TAR="gcc-${GCC_VERSION}.tar.gz"
BINUTILS_DIR="binutils-${BINUTILS_VERSION}"
GCC_DIR="gcc-${GCC_VERSION}"

echo "downloading binutils"
[[ -f $BINUTILS_TAR ]] || wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz
echo "downloading gcc"
[[ -f $GCC_TAR ]] || wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz

tar xzf $BINUTILS_TAR
tar xzf $GCC_TAR

### patch nightingale diffs
cd $BINUTILS_DIR
patch -p1 -i ../nightingale-binutils-${BINUTILS_VERSION}.patch
cd ..
cd $GCC_DIR
patch -p1 -i ../nightingale-gcc-${GCC_VERSION}.patch
cd ..

### binutils

mkdir build-binutils
cd build-binutils
../$BINUTILS_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror --with-sysroot="$SYSROOT"
make $PARALLEL
make install

### GCC

cd $BUILDDIR

mkdir build-gcc
cd build-gcc
../$GCC_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --with-sysroot="$SYSROOT"
make $PARALLEL all-gcc
make $PARALLEL all-target-libgcc
make install-gcc
make install-target-libgcc

cd $BUILDDIR
