#!/usr/bin/env bash

file="${1-build/system_root/boot/nightingale_kernel}"
addr2line_binary="llvm-addr2line"

if [ ! -f "$file" ]; then
    echo "File not found: $file"
    echo "Usage: $0 [file]"
    exit 1
fi

if grep -q '(0x.*) <.*>' last_output; then
    grep '(0x.*) <.*>' last_output | \
        sed 's/.*(0x\(.*\)) .*/\1/g' | \
        xargs "$addr2line_binary" -fips -e $file
elif grep -q '^\s\+[0-9]\+:' last_output; then
    grep '^\s\+[0-9]\+:' last_output | \
        awk '{print $7}' | \
        cut -c4- | \
        xargs "$addr2line_binary" -fips -e $file | \
        uniq -c
elif grep -q 'frame ip:' last_output; then
    grep 'frame ip:' last_output | \
        awk '{print $3}' | \
        xargs "$addr2line_binary" -fips -e $file
elif grep -q 'Fault occurred at' last_output; then
    grep 'Fault occurred at' last_output | \
        awk '{print $6}' | \
        cut -c3- | \
        xargs "$addr2line_binary" -fips -e $file
fi
