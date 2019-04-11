# gcc 8.2.0 i686-elf cross-compiler container

FROM debian:stretch
LABEL maintainer "Brett Vickers <github.com/beevik>"

ARG BINUTILS_VERSION=2.31.1
ARG GCC_VERSION=8.2.0

# Install cross-compiler prerequisites
RUN set -x \
	&& apt-get update \
	&& apt-get install -y wget gcc libgmp3-dev libmpfr-dev libisl-dev \
		libcloog-isl-dev libmpc-dev texinfo bison flex make bzip2 patch \
		build-essential

# Pull binutils and gcc source code
RUN set -x \
	&& mkdir -p /usr/local/src \
	&& cd /usr/local/src \
	&& wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz \
	&& wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz \
	&& tar zxf binutils-${BINUTILS_VERSION}.tar.gz \
	&& tar zxf gcc-${GCC_VERSION}.tar.gz \
	&& rm binutils-${BINUTILS_VERSION}.tar.gz gcc-${GCC_VERSION}.tar.gz \
	&& chown -R root:root binutils-${BINUTILS_VERSION} \
	&& chown -R root:root gcc-${GCC_VERSION} \
	&& chmod -R o-w,g+w binutils-${BINUTILS_VERSION} \
	&& chmod -R o-w,g+w gcc-${GCC_VERSION}

# Copy compile scripts
COPY files/src /usr/local/src/

# Copy gcc patches
COPY files/gcc/t-i686-elf /usr/local/src/gcc-${GCC_VERSION}/gcc/config/i386/
COPY files/gcc/config.gcc.patch /usr/local/src/gcc-${GCC_VERSION}/gcc/

# Build and install binutils and the cross-compiler
RUN set -x \
	&& cd /usr/local/src \
	&& ./build-binutils.sh ${BINUTILS_VERSION} \
	&& ./build-gcc.sh ${GCC_VERSION}

#nightingale deps
RUN DEBIAN_FRONTEND=noninteractive apt install -y grub2 xorriso mtools make nasm git

CMD ["/bin/bash"]
