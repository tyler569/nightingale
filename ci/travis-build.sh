
HOST=nightingale.philbrick.dev

mkdir -p toolchains
cd toolchains

GCC_VERSION="9.2.0-ng3"
TOOLCHAIN_FILE="toolchain-${GCC_VERSION}.txz"
TOOLCHAIN_DIR="toolchain-${GCC_VERSION}"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    wget -v "https://${HOST}/toolchains/${TOOLCHAIN_FILE}" || exit 1
fi

cd ..

tar xf toolchains/"${TOOLCHAIN_FILE}" || exit 1

export PATH="$(pwd)/${TOOLCHAIN_DIR}/bin:${PATH}"

echo "Building nightingale with toolchain ${TOOLCHAIN_DIR}"
make
