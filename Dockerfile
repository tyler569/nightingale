
FROM ubuntu:artful

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y clang llvm lld grub2 xorriso mtools make nasm

WORKDIR /nightingale
CMD ["make", "iso"]

