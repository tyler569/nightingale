#!/bin/bash

set -euo pipefail

# Prerequisite: to build gcc you will need GMP, MPFR, and MPC, on ubuntu
# the necessary packages are:
# - libgmp-dev
# - libmpfr-dev
# - libmpc-dev
#
# on fedora:
# sudo dnf install gcc gcc-c++ gmp-devel mpfr-devel libmpc-devel make bison flex wget patch

# this will tie the compiler's sysroot to the `build/sysroot` directory in your
# current checkout - make sure the project is checked out where you want it to
# stay.

pushd toolchain

# BINUTILS_VERSION="2.33.1"
# GCC_VERSION="9.2.0"
BINUTILS_VERSION="2.36.1"
GCC_VERSION="11.1.0"
PARALLEL=-j20

PREFIX="$HOME/.local"
PATH="$PREFIX/bin:$PATH"
TARGET=x86_64-nightingale

BUILDDIR=$(pwd)
NGDIR=$(realpath ..)
SYSROOT=$(realpath ../build/sysroot)

echo "building binutils ${BINUTILS_VERSION} and gcc ${GCC_VERSION} with nightingale patches"

echo "installing headers to sysroot"
pushd ..
make
popd 

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
pushd $BINUTILS_DIR
patch -p1 -i ../nightingale-binutils-${BINUTILS_VERSION}.patch
popd

pushd $GCC_DIR
patch -p1 -i ../nightingale-gcc-${GCC_VERSION}.patch
popd

### binutils

mkdir build-binutils
pushd build-binutils
../$BINUTILS_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror --with-sysroot="$SYSROOT"
make $PARALLEL
make install
popd

### GCC

mkdir build-gcc
pushd build-gcc
../$GCC_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --with-sysroot="$SYSROOT"
make $PARALLEL all-gcc
make $PARALLEL all-target-libgcc
make install-gcc
make install-target-libgcc
popd

popd # toolchain
