
mkdir -p sysroot/usr/include
mkdir -p sysroot/usr/man/man1
mkdir -p sysroot/usr/bin
mkdir -p sysroot/usr/lib
mkdir -p sysroot/usr/share
cp -LRT include sysroot/usr/include
cp -LRT libc/include sysroot/usr/include
cp -LRT kernel/include sysroot/usr/include
cp -LRT net/include sysroot/usr/include
cp -LRT linker/include sysroot/usr/include
cp -LRT external/libm/include sysroot/usr/include
