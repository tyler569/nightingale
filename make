#!/usr/bin/env bash

if [[ "$1" == "clean" ]]; then
    rm -rf build
    exit 0
fi

exec script/build_iso.bash

