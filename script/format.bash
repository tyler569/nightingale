#!/usr/bin/env bash

ARGS=("$@")
NARGS=${#ARGS[@]}

CODE="include kernel libc linker modules user"

[[ "$NARGS" -eq "0" ]] && ARGS=($(find $CODE -name '*.[ch]'))

echo ${ARGS[@]}

xargs clang-format -i <<< "${ARGS[@]}"

xargs sed -i 's/if \((.*)\) { \([^;]*;\) }/if \1 \2/' <<< "${ARGS[@]}"

# xargs sed -i 's/\(if\|while\|for\) \((.*)\) \([^;]*;\)/\1 \2  \3/' <<< "${ARGS[@]}"

# xargs sed -i 's/enum { /enum {\n\t/g' <<< "${ARGS[@]}"
