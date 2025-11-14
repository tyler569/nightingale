#!/usr/bin/env bash

file="${1-build/kernel/nightingale_kernel}"
addr2line_binary="llvm-addr2line"

if [ ! -f "$file" ]; then
    echo "File not found: $file"
    echo "Usage: $0 [file]"
    exit 1
fi

tail -n 100 last_output | \
    grep '(0x.*) <.*>' | \
    sed 's/.*(0x\(.*\)) .*/\1/g' | \
    xargs "$addr2line_binary" -fips -e $file
